import bpy
import json

def Export(outputFilename):
    out_mesh = {}
    out_mesh["vertex_structure"] = "pos,col"

    for obj in bpy.data.objects:
        if obj.type != "MESH":
            continue

        vertex_buffer = []
        index_buffer = []
        mesh = obj.data
        mesh.calc_normals_split() # compute the normal in the loop
        
        vertex_colors = mesh.vertex_colors[0].data

        vertex_index_to_out_vertex_index = {}
        vertex_index = 0
        for loop in mesh.loops:
            vertex = mesh.vertices[loop.vertex_index]
            normal = loop.normal
            color = vertex_colors[loop.index].color
            
            vertex_index_to_out_vertex_index[loop.index] = vertex_index
            vertex_index = vertex_index + 1
            
            vertex_buffer.append(vertex.co[0])        
            vertex_buffer.append(vertex.co[2])
            vertex_buffer.append(vertex.co[1])
            vertex_buffer.append(color[0])
            vertex_buffer.append(color[1])
            vertex_buffer.append(color[2])
            
        for polygon in mesh.polygons:
            loop_length = len(polygon.loop_indices)
            for ii in range(0, loop_length, 3):
                #print ("boom")
                loop_index = polygon.loop_indices[ii]
                vertex_index = vertex_index_to_out_vertex_index[loop_index]
                index_buffer.append(vertex_index)
                
                loop_index = polygon.loop_indices[ii + 2]
                vertex_index = vertex_index_to_out_vertex_index[loop_index]
                index_buffer.append(vertex_index)
                
                loop_index = polygon.loop_indices[ii + 1]
                vertex_index = vertex_index_to_out_vertex_index[loop_index]
                index_buffer.append(vertex_index)
                    
        out_mesh["vertex_buffer"] = vertex_buffer
        out_mesh["triangle_count"] = int(len(index_buffer) / 3)
        out_mesh["index_buffer"] = index_buffer
        
        with open(outputFilename, 'w') as out_file:
            json.dump(out_mesh, out_file, indent = 2)

current_filename = bpy.data.filepath
filename = bpy.path.basename(current_filename)
filename = filename.split('.')[0]
output_root = "C:/workspace/Alpha/data/mesh"
output = output_root + "/" + filename + ".json"
Export(output)
print ("%s export over" % output)
