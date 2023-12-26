import argparse
import bmesh
import bpy
import json
import sys

def Triangulate(obj):
	#switch to object mode to triangulate
	bpy.ops.object.mode_set(mode='OBJECT')

	mesh = obj.data
	bm = bmesh.new()
	bm.from_mesh(mesh)
	bmesh.ops.triangulate(bm, faces=bm.faces[:])
	
	bm.to_mesh(mesh)
	bm.free()


def Export(outputFilename):

	out_mesh = {}
	out_mesh["vertex_structure"] = "pos,col,uv"

	#outputs
	vertex_buffer = []
	index_buffer = []
	vertex_index = 0

	for obj in bpy.data.objects:
		if obj.type != "MESH":
			continue

		#set the active object to the mesh
		bpy.context.view_layer.objects.active = obj

		Triangulate(obj)
		
		#switch to vertex paint mode otherwise the vertex color are not applied to the vertices.
		bpy.ops.object.mode_set(mode='VERTEX_PAINT')
	
		txWs = obj.matrix_world
		mesh = obj.data
		mesh.calc_normals_split() # compute the normal in the loop
		
		vertex_colors = None

		if len(mesh.vertex_colors) > 0:
			vertex_colors = mesh.vertex_colors[0].data

		vertex_uvlayers = mesh.uv_layers[0].data

		vertex_index_to_out_vertex_index = {}
		
		for loop in mesh.loops:
			vertex = mesh.vertices[loop.vertex_index]
			normal = loop.normal

			color = [1, 1, 1]
			if vertex_colors is not None:
				color = vertex_colors[loop.index].color

			uv = vertex_uvlayers[loop.index].uv
			vertex_index_to_out_vertex_index[loop.index] = vertex_index
			vertex_index = vertex_index + 1
			
			vertexLs = vertex.co
			vertexWs = txWs @ vertexLs
			
			vertex_buffer.append(vertexWs[0])
			vertex_buffer.append(vertexWs[2])
			vertex_buffer.append(vertexWs[1])
			vertex_buffer.append(color[0])
			vertex_buffer.append(color[1])
			vertex_buffer.append(color[2])
			vertex_buffer.append(uv[0])
			vertex_buffer.append(uv[1])
			
		for polygon in mesh.polygons:
			loop_length = len(polygon.loop_indices)
			for ii in range(0, loop_length, 3):
				loop_index = polygon.loop_indices[ii]
				current_vertex_index = vertex_index_to_out_vertex_index[loop_index]
				index_buffer.append(current_vertex_index)
				
				loop_index = polygon.loop_indices[ii + 2]
				current_vertex_index = vertex_index_to_out_vertex_index[loop_index]
				index_buffer.append(current_vertex_index)
				
				loop_index = polygon.loop_indices[ii + 1]
				current_vertex_index = vertex_index_to_out_vertex_index[loop_index]
				index_buffer.append(current_vertex_index)

	out_mesh["vertex_buffer"] = vertex_buffer
	out_mesh["triangle_count"] = int(len(index_buffer) / 3)
	out_mesh["index_buffer"] = index_buffer
		
	with open(outputFilename, 'w') as out_file:
		print("writing file " + outputFilename)
		json.dump(out_mesh, out_file, indent = 2)


print("Start Export...")
#parse arguments after --. Arguments before -- are for blender, after are for the python script.
# command line example : blender --background c:\workspace\Alpha\raw\blender\base_torus.blend --python c:\workspace\Alpha\code\scripts\export_mesh.py -- --id 3
argv = sys.argv
argv = argv[argv.index("--") + 1:]	# get all args after "--"

parser = argparse.ArgumentParser(description="Convert a mesh into a json file.")
parser.add_argument("--id", dest="assetId", action="store", help="Id of the asset where to export this mesh.")

args = parser.parse_args(argv)

#create the export filename
filename = str(args.assetId).zfill(8)
output_root = "C:/workspace/Alpha/data/mesh"
output = output_root + "/" + filename

#export mesh
Export(output)
print ("%s export over" % output)
