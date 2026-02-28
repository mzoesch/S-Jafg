import bpy
op = bpy.context.active_operator

# Common
op.export_format = 'GLB'

# Transform
op.export_yup = True

###############################################################################
# Data
###############################################################################
# Scene Graph
op.export_gpu_instances = False
op.export_hierarchy_flatten_objs = False
op.export_hierarchy_full_collections = False

# Mesh
op.export_apply = False
op.export_texcoords = True
op.export_normals = True
op.export_tangents = True
op.export_attributes = False
op.export_shared_accessors = False

# Vertex Colors
op.export_vertex_color = 'NONE'

# Material
op.export_materials = 'NONE'

# Armature
op.export_rest_position_armature = True
op.export_def_bones = False
op.export_armature_object_remove = True
op.export_hierarchy_flatten_bones = False
op.export_leaf_bone = False

# Lighting
op.export_import_convert_lighting_mode = 'SPEC'

# Compression
op.export_draco_mesh_compression_enable = False

# Animation
op.export_animations = False
