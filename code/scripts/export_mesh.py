import bpy
import json

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
        
        vertex_buffer.append(vertex.co[0]) #x is right       
        vertex_buffer.append(vertex.co[2]) #z is up
        vertex_buffer.append(vertex.co[1]) #y is forward
        vertex_buffer.append(color[0])
        vertex_buffer.append(color[1])
        vertex_buffer.append(color[2])
        
    for polygon in mesh.polygons:
        count = len(polygon.loop_indices)
        if count != 3:
            print ("Triangulate the mesh!")
            exit()
        
        loop_indices = polygon.loop_indices    
        vertex_index = vertex_index_to_out_vertex_index[loop_indices[0]]
        index_buffer.append(vertex_index)
        vertex_index = vertex_index_to_out_vertex_index[loop_indices[2]]
        index_buffer.append(vertex_index)
        vertex_index = vertex_index_to_out_vertex_index[loop_indices[1]]
        index_buffer.append(vertex_index)
        
    out_mesh["vertex_buffer"] = vertex_buffer
    out_mesh["index_buffer"] = index_buffer
    
    with open("c:/tmp/plane.json", 'w') as out_file:
        json.dump(out_mesh, out_file, indent = 2)

print ("Over")
