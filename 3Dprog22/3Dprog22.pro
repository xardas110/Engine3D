QT          += core gui widgets opengl

TEMPLATE    = app
CONFIG      += c++17

TARGET      = 3D-programmering

SOURCES += main.cpp \
    AnimationHelper.cpp \
    Camera.cpp \
    CascadedShadowMaps.cpp \
    Character.cpp \
    CollisionHelper.cpp \
    CollisionManager.cpp \
    CollisionSystem.cpp \
    CollisionVolume.cpp \
    Entity.cpp \
    FrameBufferManager.cpp \
    Json.cpp \
    MeshManager.cpp \
    Octree.cpp \
    PhysicsSystem.cpp \
    PointGenerator.cpp \
    Raycast.cpp \
    RenderSystem.cpp \
    ShaderManager.cpp \
    SkeletalMesh.cpp \
    SkeletalMeshManager.cpp \
    StaticMesh.cpp \
    StaticMeshManager.cpp \
    UBOManager.cpp \
    World.cpp \
    WorldSerializer.cpp \
    editor.cpp \
    lissajous.cpp \
    logger.cpp \
    mainwindow.cpp \
    renderwindow.cpp \
    shader.cpp \
    skybox.cpp \
    texturemanager.cpp \
    trianglesurface.cpp \
    vertex.cpp \
    visualcube.cpp \
    visualobject.cpp

HEADERS += \
    AnimationHelper.h \
    CMD.h \
    Camera.h \
    CascadedShadowMaps.h \
    Character.h \
    CollisionHelper.h \
    CollisionManager.h \
    CollisionSystem.h \
    CollisionVolume.h \
    Components.h \
    CpuProfiler.h \
    Debug.h \
    DirectionalLight.h \
    Entity.h \
    FrameBuffer.h \
    FrameBufferManager.h \
    GlobalTypeDefs.h \
    HelperMath.h \
    Include/glm/common.hpp \
    Include/glm/detail/_features.hpp \
    Include/glm/detail/_fixes.hpp \
    Include/glm/detail/_noise.hpp \
    Include/glm/detail/_swizzle.hpp \
    Include/glm/detail/_swizzle_func.hpp \
    Include/glm/detail/_vectorize.hpp \
    Include/glm/detail/compute_common.hpp \
    Include/glm/detail/compute_vector_relational.hpp \
    Include/glm/detail/func_common.inl \
    Include/glm/detail/func_common_simd.inl \
    Include/glm/detail/func_exponential.inl \
    Include/glm/detail/func_exponential_simd.inl \
    Include/glm/detail/func_geometric.inl \
    Include/glm/detail/func_geometric_simd.inl \
    Include/glm/detail/func_integer.inl \
    Include/glm/detail/func_integer_simd.inl \
    Include/glm/detail/func_matrix.inl \
    Include/glm/detail/func_matrix_simd.inl \
    Include/glm/detail/func_packing.inl \
    Include/glm/detail/func_packing_simd.inl \
    Include/glm/detail/func_trigonometric.inl \
    Include/glm/detail/func_trigonometric_simd.inl \
    Include/glm/detail/func_vector_relational.inl \
    Include/glm/detail/func_vector_relational_simd.inl \
    Include/glm/detail/qualifier.hpp \
    Include/glm/detail/setup.hpp \
    Include/glm/detail/type_float.hpp \
    Include/glm/detail/type_half.hpp \
    Include/glm/detail/type_half.inl \
    Include/glm/detail/type_mat2x2.hpp \
    Include/glm/detail/type_mat2x2.inl \
    Include/glm/detail/type_mat2x3.hpp \
    Include/glm/detail/type_mat2x3.inl \
    Include/glm/detail/type_mat2x4.hpp \
    Include/glm/detail/type_mat2x4.inl \
    Include/glm/detail/type_mat3x2.hpp \
    Include/glm/detail/type_mat3x2.inl \
    Include/glm/detail/type_mat3x3.hpp \
    Include/glm/detail/type_mat3x3.inl \
    Include/glm/detail/type_mat3x4.hpp \
    Include/glm/detail/type_mat3x4.inl \
    Include/glm/detail/type_mat4x2.hpp \
    Include/glm/detail/type_mat4x2.inl \
    Include/glm/detail/type_mat4x3.hpp \
    Include/glm/detail/type_mat4x3.inl \
    Include/glm/detail/type_mat4x4.hpp \
    Include/glm/detail/type_mat4x4.inl \
    Include/glm/detail/type_mat4x4_simd.inl \
    Include/glm/detail/type_quat.hpp \
    Include/glm/detail/type_quat.inl \
    Include/glm/detail/type_quat_simd.inl \
    Include/glm/detail/type_vec1.hpp \
    Include/glm/detail/type_vec1.inl \
    Include/glm/detail/type_vec2.hpp \
    Include/glm/detail/type_vec2.inl \
    Include/glm/detail/type_vec3.hpp \
    Include/glm/detail/type_vec3.inl \
    Include/glm/detail/type_vec4.hpp \
    Include/glm/detail/type_vec4.inl \
    Include/glm/detail/type_vec4_simd.inl \
    Include/glm/exponential.hpp \
    Include/glm/ext/matrix_clip_space.hpp \
    Include/glm/ext/matrix_clip_space.inl \
    Include/glm/ext/matrix_common.hpp \
    Include/glm/ext/matrix_common.inl \
    Include/glm/ext/matrix_double2x2.hpp \
    Include/glm/ext/matrix_double2x2_precision.hpp \
    Include/glm/ext/matrix_double2x3.hpp \
    Include/glm/ext/matrix_double2x3_precision.hpp \
    Include/glm/ext/matrix_double2x4.hpp \
    Include/glm/ext/matrix_double2x4_precision.hpp \
    Include/glm/ext/matrix_double3x2.hpp \
    Include/glm/ext/matrix_double3x2_precision.hpp \
    Include/glm/ext/matrix_double3x3.hpp \
    Include/glm/ext/matrix_double3x3_precision.hpp \
    Include/glm/ext/matrix_double3x4.hpp \
    Include/glm/ext/matrix_double3x4_precision.hpp \
    Include/glm/ext/matrix_double4x2.hpp \
    Include/glm/ext/matrix_double4x2_precision.hpp \
    Include/glm/ext/matrix_double4x3.hpp \
    Include/glm/ext/matrix_double4x3_precision.hpp \
    Include/glm/ext/matrix_double4x4.hpp \
    Include/glm/ext/matrix_double4x4_precision.hpp \
    Include/glm/ext/matrix_float2x2.hpp \
    Include/glm/ext/matrix_float2x2_precision.hpp \
    Include/glm/ext/matrix_float2x3.hpp \
    Include/glm/ext/matrix_float2x3_precision.hpp \
    Include/glm/ext/matrix_float2x4.hpp \
    Include/glm/ext/matrix_float2x4_precision.hpp \
    Include/glm/ext/matrix_float3x2.hpp \
    Include/glm/ext/matrix_float3x2_precision.hpp \
    Include/glm/ext/matrix_float3x3.hpp \
    Include/glm/ext/matrix_float3x3_precision.hpp \
    Include/glm/ext/matrix_float3x4.hpp \
    Include/glm/ext/matrix_float3x4_precision.hpp \
    Include/glm/ext/matrix_float4x2.hpp \
    Include/glm/ext/matrix_float4x2_precision.hpp \
    Include/glm/ext/matrix_float4x3.hpp \
    Include/glm/ext/matrix_float4x3_precision.hpp \
    Include/glm/ext/matrix_float4x4.hpp \
    Include/glm/ext/matrix_float4x4_precision.hpp \
    Include/glm/ext/matrix_int2x2.hpp \
    Include/glm/ext/matrix_int2x2_sized.hpp \
    Include/glm/ext/matrix_int2x3.hpp \
    Include/glm/ext/matrix_int2x3_sized.hpp \
    Include/glm/ext/matrix_int2x4.hpp \
    Include/glm/ext/matrix_int2x4_sized.hpp \
    Include/glm/ext/matrix_int3x2.hpp \
    Include/glm/ext/matrix_int3x2_sized.hpp \
    Include/glm/ext/matrix_int3x3.hpp \
    Include/glm/ext/matrix_int3x3_sized.hpp \
    Include/glm/ext/matrix_int3x4.hpp \
    Include/glm/ext/matrix_int3x4_sized.hpp \
    Include/glm/ext/matrix_int4x2.hpp \
    Include/glm/ext/matrix_int4x2_sized.hpp \
    Include/glm/ext/matrix_int4x3.hpp \
    Include/glm/ext/matrix_int4x3_sized.hpp \
    Include/glm/ext/matrix_int4x4.hpp \
    Include/glm/ext/matrix_int4x4_sized.hpp \
    Include/glm/ext/matrix_projection.hpp \
    Include/glm/ext/matrix_projection.inl \
    Include/glm/ext/matrix_relational.hpp \
    Include/glm/ext/matrix_relational.inl \
    Include/glm/ext/matrix_transform.hpp \
    Include/glm/ext/matrix_transform.inl \
    Include/glm/ext/matrix_uint2x2.hpp \
    Include/glm/ext/matrix_uint2x2_sized.hpp \
    Include/glm/ext/matrix_uint2x3.hpp \
    Include/glm/ext/matrix_uint2x3_sized.hpp \
    Include/glm/ext/matrix_uint2x4.hpp \
    Include/glm/ext/matrix_uint2x4_sized.hpp \
    Include/glm/ext/matrix_uint3x2.hpp \
    Include/glm/ext/matrix_uint3x2_sized.hpp \
    Include/glm/ext/matrix_uint3x3.hpp \
    Include/glm/ext/matrix_uint3x3_sized.hpp \
    Include/glm/ext/matrix_uint3x4.hpp \
    Include/glm/ext/matrix_uint3x4_sized.hpp \
    Include/glm/ext/matrix_uint4x2.hpp \
    Include/glm/ext/matrix_uint4x2_sized.hpp \
    Include/glm/ext/matrix_uint4x3.hpp \
    Include/glm/ext/matrix_uint4x3_sized.hpp \
    Include/glm/ext/matrix_uint4x4.hpp \
    Include/glm/ext/matrix_uint4x4_sized.hpp \
    Include/glm/ext/quaternion_common.hpp \
    Include/glm/ext/quaternion_common.inl \
    Include/glm/ext/quaternion_common_simd.inl \
    Include/glm/ext/quaternion_double.hpp \
    Include/glm/ext/quaternion_double_precision.hpp \
    Include/glm/ext/quaternion_exponential.hpp \
    Include/glm/ext/quaternion_exponential.inl \
    Include/glm/ext/quaternion_float.hpp \
    Include/glm/ext/quaternion_float_precision.hpp \
    Include/glm/ext/quaternion_geometric.hpp \
    Include/glm/ext/quaternion_geometric.inl \
    Include/glm/ext/quaternion_relational.hpp \
    Include/glm/ext/quaternion_relational.inl \
    Include/glm/ext/quaternion_transform.hpp \
    Include/glm/ext/quaternion_transform.inl \
    Include/glm/ext/quaternion_trigonometric.hpp \
    Include/glm/ext/quaternion_trigonometric.inl \
    Include/glm/ext/scalar_common.hpp \
    Include/glm/ext/scalar_common.inl \
    Include/glm/ext/scalar_constants.hpp \
    Include/glm/ext/scalar_constants.inl \
    Include/glm/ext/scalar_int_sized.hpp \
    Include/glm/ext/scalar_integer.hpp \
    Include/glm/ext/scalar_integer.inl \
    Include/glm/ext/scalar_packing.hpp \
    Include/glm/ext/scalar_packing.inl \
    Include/glm/ext/scalar_relational.hpp \
    Include/glm/ext/scalar_relational.inl \
    Include/glm/ext/scalar_uint_sized.hpp \
    Include/glm/ext/scalar_ulp.hpp \
    Include/glm/ext/scalar_ulp.inl \
    Include/glm/ext/vector_bool1.hpp \
    Include/glm/ext/vector_bool1_precision.hpp \
    Include/glm/ext/vector_bool2.hpp \
    Include/glm/ext/vector_bool2_precision.hpp \
    Include/glm/ext/vector_bool3.hpp \
    Include/glm/ext/vector_bool3_precision.hpp \
    Include/glm/ext/vector_bool4.hpp \
    Include/glm/ext/vector_bool4_precision.hpp \
    Include/glm/ext/vector_common.hpp \
    Include/glm/ext/vector_common.inl \
    Include/glm/ext/vector_double1.hpp \
    Include/glm/ext/vector_double1_precision.hpp \
    Include/glm/ext/vector_double2.hpp \
    Include/glm/ext/vector_double2_precision.hpp \
    Include/glm/ext/vector_double3.hpp \
    Include/glm/ext/vector_double3_precision.hpp \
    Include/glm/ext/vector_double4.hpp \
    Include/glm/ext/vector_double4_precision.hpp \
    Include/glm/ext/vector_float1.hpp \
    Include/glm/ext/vector_float1_precision.hpp \
    Include/glm/ext/vector_float2.hpp \
    Include/glm/ext/vector_float2_precision.hpp \
    Include/glm/ext/vector_float3.hpp \
    Include/glm/ext/vector_float3_precision.hpp \
    Include/glm/ext/vector_float4.hpp \
    Include/glm/ext/vector_float4_precision.hpp \
    Include/glm/ext/vector_int1.hpp \
    Include/glm/ext/vector_int1_sized.hpp \
    Include/glm/ext/vector_int2.hpp \
    Include/glm/ext/vector_int2_sized.hpp \
    Include/glm/ext/vector_int3.hpp \
    Include/glm/ext/vector_int3_sized.hpp \
    Include/glm/ext/vector_int4.hpp \
    Include/glm/ext/vector_int4_sized.hpp \
    Include/glm/ext/vector_integer.hpp \
    Include/glm/ext/vector_integer.inl \
    Include/glm/ext/vector_packing.hpp \
    Include/glm/ext/vector_packing.inl \
    Include/glm/ext/vector_relational.hpp \
    Include/glm/ext/vector_relational.inl \
    Include/glm/ext/vector_uint1.hpp \
    Include/glm/ext/vector_uint1_sized.hpp \
    Include/glm/ext/vector_uint2.hpp \
    Include/glm/ext/vector_uint2_sized.hpp \
    Include/glm/ext/vector_uint3.hpp \
    Include/glm/ext/vector_uint3_sized.hpp \
    Include/glm/ext/vector_uint4.hpp \
    Include/glm/ext/vector_uint4_sized.hpp \
    Include/glm/ext/vector_ulp.hpp \
    Include/glm/ext/vector_ulp.inl \
    Include/glm/fwd.hpp \
    Include/glm/geometric.hpp \
    Include/glm/glm.hpp \
    Include/glm/gtc/bitfield.hpp \
    Include/glm/gtc/bitfield.inl \
    Include/glm/gtc/color_space.hpp \
    Include/glm/gtc/color_space.inl \
    Include/glm/gtc/constants.hpp \
    Include/glm/gtc/constants.inl \
    Include/glm/gtc/epsilon.hpp \
    Include/glm/gtc/epsilon.inl \
    Include/glm/gtc/integer.hpp \
    Include/glm/gtc/integer.inl \
    Include/glm/gtc/matrix_access.hpp \
    Include/glm/gtc/matrix_access.inl \
    Include/glm/gtc/matrix_integer.hpp \
    Include/glm/gtc/matrix_inverse.hpp \
    Include/glm/gtc/matrix_inverse.inl \
    Include/glm/gtc/matrix_transform.hpp \
    Include/glm/gtc/matrix_transform.inl \
    Include/glm/gtc/noise.hpp \
    Include/glm/gtc/noise.inl \
    Include/glm/gtc/packing.hpp \
    Include/glm/gtc/packing.inl \
    Include/glm/gtc/quaternion.hpp \
    Include/glm/gtc/quaternion.inl \
    Include/glm/gtc/quaternion_simd.inl \
    Include/glm/gtc/random.hpp \
    Include/glm/gtc/random.inl \
    Include/glm/gtc/reciprocal.hpp \
    Include/glm/gtc/reciprocal.inl \
    Include/glm/gtc/round.hpp \
    Include/glm/gtc/round.inl \
    Include/glm/gtc/type_aligned.hpp \
    Include/glm/gtc/type_precision.hpp \
    Include/glm/gtc/type_precision.inl \
    Include/glm/gtc/type_ptr.hpp \
    Include/glm/gtc/type_ptr.inl \
    Include/glm/gtc/ulp.hpp \
    Include/glm/gtc/ulp.inl \
    Include/glm/gtc/vec1.hpp \
    Include/glm/gtx/associated_min_max.hpp \
    Include/glm/gtx/associated_min_max.inl \
    Include/glm/gtx/bit.hpp \
    Include/glm/gtx/bit.inl \
    Include/glm/gtx/closest_point.hpp \
    Include/glm/gtx/closest_point.inl \
    Include/glm/gtx/color_encoding.hpp \
    Include/glm/gtx/color_encoding.inl \
    Include/glm/gtx/color_space.hpp \
    Include/glm/gtx/color_space.inl \
    Include/glm/gtx/color_space_YCoCg.hpp \
    Include/glm/gtx/color_space_YCoCg.inl \
    Include/glm/gtx/common.hpp \
    Include/glm/gtx/common.inl \
    Include/glm/gtx/compatibility.hpp \
    Include/glm/gtx/compatibility.inl \
    Include/glm/gtx/component_wise.hpp \
    Include/glm/gtx/component_wise.inl \
    Include/glm/gtx/dual_quaternion.hpp \
    Include/glm/gtx/dual_quaternion.inl \
    Include/glm/gtx/easing.hpp \
    Include/glm/gtx/easing.inl \
    Include/glm/gtx/euler_angles.hpp \
    Include/glm/gtx/euler_angles.inl \
    Include/glm/gtx/extend.hpp \
    Include/glm/gtx/extend.inl \
    Include/glm/gtx/extended_min_max.hpp \
    Include/glm/gtx/extended_min_max.inl \
    Include/glm/gtx/exterior_product.hpp \
    Include/glm/gtx/exterior_product.inl \
    Include/glm/gtx/fast_exponential.hpp \
    Include/glm/gtx/fast_exponential.inl \
    Include/glm/gtx/fast_square_root.hpp \
    Include/glm/gtx/fast_square_root.inl \
    Include/glm/gtx/fast_trigonometry.hpp \
    Include/glm/gtx/fast_trigonometry.inl \
    Include/glm/gtx/float_notmalize.inl \
    Include/glm/gtx/functions.hpp \
    Include/glm/gtx/functions.inl \
    Include/glm/gtx/gradient_paint.hpp \
    Include/glm/gtx/gradient_paint.inl \
    Include/glm/gtx/handed_coordinate_space.hpp \
    Include/glm/gtx/handed_coordinate_space.inl \
    Include/glm/gtx/hash.hpp \
    Include/glm/gtx/hash.inl \
    Include/glm/gtx/integer.hpp \
    Include/glm/gtx/integer.inl \
    Include/glm/gtx/intersect.hpp \
    Include/glm/gtx/intersect.inl \
    Include/glm/gtx/io.hpp \
    Include/glm/gtx/io.inl \
    Include/glm/gtx/log_base.hpp \
    Include/glm/gtx/log_base.inl \
    Include/glm/gtx/matrix_cross_product.hpp \
    Include/glm/gtx/matrix_cross_product.inl \
    Include/glm/gtx/matrix_decompose.hpp \
    Include/glm/gtx/matrix_decompose.inl \
    Include/glm/gtx/matrix_factorisation.hpp \
    Include/glm/gtx/matrix_factorisation.inl \
    Include/glm/gtx/matrix_interpolation.hpp \
    Include/glm/gtx/matrix_interpolation.inl \
    Include/glm/gtx/matrix_major_storage.hpp \
    Include/glm/gtx/matrix_major_storage.inl \
    Include/glm/gtx/matrix_operation.hpp \
    Include/glm/gtx/matrix_operation.inl \
    Include/glm/gtx/matrix_query.hpp \
    Include/glm/gtx/matrix_query.inl \
    Include/glm/gtx/matrix_transform_2d.hpp \
    Include/glm/gtx/matrix_transform_2d.inl \
    Include/glm/gtx/mixed_product.hpp \
    Include/glm/gtx/mixed_product.inl \
    Include/glm/gtx/norm.hpp \
    Include/glm/gtx/norm.inl \
    Include/glm/gtx/normal.hpp \
    Include/glm/gtx/normal.inl \
    Include/glm/gtx/normalize_dot.hpp \
    Include/glm/gtx/normalize_dot.inl \
    Include/glm/gtx/number_precision.hpp \
    Include/glm/gtx/number_precision.inl \
    Include/glm/gtx/optimum_pow.hpp \
    Include/glm/gtx/optimum_pow.inl \
    Include/glm/gtx/orthonormalize.hpp \
    Include/glm/gtx/orthonormalize.inl \
    Include/glm/gtx/perpendicular.hpp \
    Include/glm/gtx/perpendicular.inl \
    Include/glm/gtx/polar_coordinates.hpp \
    Include/glm/gtx/polar_coordinates.inl \
    Include/glm/gtx/projection.hpp \
    Include/glm/gtx/projection.inl \
    Include/glm/gtx/quaternion.hpp \
    Include/glm/gtx/quaternion.inl \
    Include/glm/gtx/range.hpp \
    Include/glm/gtx/raw_data.hpp \
    Include/glm/gtx/raw_data.inl \
    Include/glm/gtx/rotate_normalized_axis.hpp \
    Include/glm/gtx/rotate_normalized_axis.inl \
    Include/glm/gtx/rotate_vector.hpp \
    Include/glm/gtx/rotate_vector.inl \
    Include/glm/gtx/scalar_multiplication.hpp \
    Include/glm/gtx/scalar_relational.hpp \
    Include/glm/gtx/scalar_relational.inl \
    Include/glm/gtx/spline.hpp \
    Include/glm/gtx/spline.inl \
    Include/glm/gtx/std_based_type.hpp \
    Include/glm/gtx/std_based_type.inl \
    Include/glm/gtx/string_cast.hpp \
    Include/glm/gtx/string_cast.inl \
    Include/glm/gtx/texture.hpp \
    Include/glm/gtx/texture.inl \
    Include/glm/gtx/transform.hpp \
    Include/glm/gtx/transform.inl \
    Include/glm/gtx/transform2.hpp \
    Include/glm/gtx/transform2.inl \
    Include/glm/gtx/type_aligned.hpp \
    Include/glm/gtx/type_aligned.inl \
    Include/glm/gtx/type_trait.hpp \
    Include/glm/gtx/type_trait.inl \
    Include/glm/gtx/vec_swizzle.hpp \
    Include/glm/gtx/vector_angle.hpp \
    Include/glm/gtx/vector_angle.inl \
    Include/glm/gtx/vector_query.hpp \
    Include/glm/gtx/vector_query.inl \
    Include/glm/gtx/wrap.hpp \
    Include/glm/gtx/wrap.inl \
    Include/glm/integer.hpp \
    Include/glm/mat2x2.hpp \
    Include/glm/mat2x3.hpp \
    Include/glm/mat2x4.hpp \
    Include/glm/mat3x2.hpp \
    Include/glm/mat3x3.hpp \
    Include/glm/mat3x4.hpp \
    Include/glm/mat4x2.hpp \
    Include/glm/mat4x3.hpp \
    Include/glm/mat4x4.hpp \
    Include/glm/matrix.hpp \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/ShowIncludes/foo.h \
    Include/glm/packing.hpp \
    Include/glm/simd/common.h \
    Include/glm/simd/exponential.h \
    Include/glm/simd/geometric.h \
    Include/glm/simd/integer.h \
    Include/glm/simd/matrix.h \
    Include/glm/simd/neon.h \
    Include/glm/simd/packing.h \
    Include/glm/simd/platform.h \
    Include/glm/simd/trigonometric.h \
    Include/glm/simd/vector_relational.h \
    Include/glm/trigonometric.hpp \
    Include/glm/vec2.hpp \
    Include/glm/vec3.hpp \
    Include/glm/vec4.hpp \
    Include/glm/vector_relational.hpp \
    Json.h \
    Material.h \
    Mesh.h \
    MeshManager.h \
    Model.h \
    Octree.h \
    PhysicsHelper.h \
    PhysicsSystem.h \
    PointGenerator.h \
    Raycast.h \
    RenderSystem.h \
    RigidBody.h \
    ShaderManager.h \
    ShaderParameters.h \
    SkeletalMesh.h \
    SkeletalMeshManager.h \
    StaticMesh.h \
    StaticMeshManager.h \
    Texture.h \
    UBO.h \
    UBOManager.h \
    World.h \
    WorldSerializer.h \
    editor.h \
    lissajous.h \
    logger.h \
    mainwindow.h \
    renderwindow.h \
    shader.h \
    skybox.h \
    stb_image.h \
    texturemanager.h \
    trianglesurface.h \
    ui_mainwindow.h \
    vertex.h \
    visualcube.h \
    visualobject.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    Assets/Maps/HeightFields/HM.RAW \
    Assets/Maps/HeightFields/Testing.txt \
    Assets/Maps/HeightFields/output.r16 \
    Assets/Maps/HeightFields/terrain0-16bbp-257x257.raw \
    Assets/Maps/Textures/grass_field.jpg \
    Assets/Models/Char3/Boxing.dae \
    Assets/Models/Char3/Char3.dae \
    Assets/Models/Char3/Char3.mtl \
    Assets/Models/Char3/Char4.dae \
    Assets/Models/Char3/sheath sword 2.dae \
    Assets/Models/Char3/sheathsword1.dae \
    Assets/Models/Char3/sword and shield 180 turn (2).dae \
    Assets/Models/Char3/sword and shield 180 turn.dae \
    Assets/Models/Char3/sword and shield attack (2).dae \
    Assets/Models/Char3/sword and shield attack (3).dae \
    Assets/Models/Char3/sword and shield attack (4).dae \
    Assets/Models/Char3/sword and shield attack.dae \
    Assets/Models/Char3/sword and shield block (2).dae \
    Assets/Models/Char3/sword and shield block idle.dae \
    Assets/Models/Char3/sword and shield block.dae \
    Assets/Models/Char3/sword and shield casting (2).dae \
    Assets/Models/Char3/sword and shield casting.dae \
    Assets/Models/Char3/sword and shield crouch block (2).dae \
    Assets/Models/Char3/sword and shield crouch block idle.dae \
    Assets/Models/Char3/sword and shield crouch block.dae \
    Assets/Models/Char3/sword and shield crouch idle.dae \
    Assets/Models/Char3/sword and shield crouch.dae \
    Assets/Models/Char3/sword and shield crouching (2).dae \
    Assets/Models/Char3/sword and shield crouching (3).dae \
    Assets/Models/Char3/sword and shield crouching.dae \
    Assets/Models/Char3/sword and shield death (2).dae \
    Assets/Models/Char3/sword and shield death.dae \
    Assets/Models/Char3/sword and shield idle (2).dae \
    Assets/Models/Char3/sword and shield idle (3).dae \
    Assets/Models/Char3/sword and shield idle (4).dae \
    Assets/Models/Char3/sword and shield idle.dae \
    Assets/Models/Char3/sword and shield impact (2).dae \
    Assets/Models/Char3/sword and shield impact (3).dae \
    Assets/Models/Char3/sword and shield impact.dae \
    Assets/Models/Char3/sword and shield jump (2).dae \
    Assets/Models/Char3/sword and shield jump.dae \
    Assets/Models/Char3/sword and shield kick.dae \
    Assets/Models/Char3/sword and shield power up.dae \
    Assets/Models/Char3/sword and shield run (2).dae \
    Assets/Models/Char3/sword and shield run.dae \
    Assets/Models/Char3/sword and shield slash (2).dae \
    Assets/Models/Char3/sword and shield slash (3).dae \
    Assets/Models/Char3/sword and shield slash (4).dae \
    Assets/Models/Char3/sword and shield slash (5).dae \
    Assets/Models/Char3/sword and shield slash.dae \
    Assets/Models/Char3/sword and shield strafe (2).dae \
    Assets/Models/Char3/sword and shield strafe (3).dae \
    Assets/Models/Char3/sword and shield strafe (4).dae \
    Assets/Models/Char3/sword and shield strafe.dae \
    Assets/Models/Char3/sword and shield turn (2).dae \
    Assets/Models/Char3/sword and shield turn.dae \
    Assets/Models/Char3/sword and shield walk (2).dae \
    Assets/Models/Char3/sword and shield walk.dae \
    Assets/Models/Char3/textures/Char3_Body_Diffuse.png \
    Assets/Models/Char3/textures/Char3_Body_Gloss.png \
    Assets/Models/Char3/textures/Char3_Body_Normal.png \
    Assets/Models/Char3/textures/Char3_Body_Opacity.png \
    Assets/Models/Char3/textures/Char3_Body_Specular.png \
    Assets/Models/IronMan/IronMan.mtl \
    Assets/Models/IronMan/v1_0_IronManRigged.max \
    Assets/Models/alissa/alissa.dae \
    Assets/Models/alissa/animations/ArmsHipHopDance.dae \
    Assets/Models/alissa/textures/Arissa_DIFF_diffuse.png \
    Assets/Models/alissa/textures/Arissa_NM_normal.png \
    Assets/Models/alissa/textures/Arissa_SPEC_specular.png \
    Assets/Models/maw/animations/SnakeHipHopDance.dae \
    Assets/Models/maw/maw.dae \
    Assets/Models/maw/textures/MAW_diffuse.png \
    Assets/Models/maw/textures/MAW_glow.png \
    Assets/Models/maw/textures/MAW_normal.png \
    Assets/Models/maw/textures/maria_specular.png \
    Assets/Models/nanosuit/LICENSE.txt \
    Assets/Models/nanosuit/arm_dif.png \
    Assets/Models/nanosuit/arm_showroom_ddn.png \
    Assets/Models/nanosuit/arm_showroom_spec.png \
    Assets/Models/nanosuit/body_dif.png \
    Assets/Models/nanosuit/body_showroom_ddn.png \
    Assets/Models/nanosuit/body_showroom_spec.png \
    Assets/Models/nanosuit/glass_ddn.png \
    Assets/Models/nanosuit/glass_dif.png \
    Assets/Models/nanosuit/hand_dif.png \
    Assets/Models/nanosuit/hand_showroom_ddn.png \
    Assets/Models/nanosuit/hand_showroom_spec.png \
    Assets/Models/nanosuit/helmet_diff.png \
    Assets/Models/nanosuit/helmet_showroom_ddn.png \
    Assets/Models/nanosuit/helmet_showroom_spec.png \
    Assets/Models/nanosuit/leg_dif.png \
    Assets/Models/nanosuit/leg_showroom_ddn.png \
    Assets/Models/nanosuit/leg_showroom_spec.png \
    Assets/Models/nanosuit/nanosuit.blend \
    Assets/Models/nanosuit/nanosuit.mtl \
    Assets/Models/nightshade/animations/HipHopDancing.dae \
    Assets/Models/nightshade/animations/Idle.dae \
    Assets/Models/nightshade/animations/Running.fbx \
    Assets/Models/nightshade/nightshade.dae \
    Assets/Models/nightshade/textures/Nightshade_diffuse.png \
    Assets/Models/nightshade/textures/Nightshade_glow.png \
    Assets/Models/nightshade/textures/Nightshade_normal.png \
    Assets/Models/nightshade/textures/Nightshade_specular.png \
    Assets/Textures/4K_Space_Skyboxes/Demo/Demo_scene.unity \
    Assets/Textures/4K_Space_Skyboxes/Demo/Image/space_skyboxes_logo.png \
    Assets/Textures/4K_Space_Skyboxes/Demo/Scripts/Camera_control.cs \
    Assets/Textures/4K_Space_Skyboxes/Demo/Scripts/Skybox_rotator.cs \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Medium_Blue_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Textures/Medium_Blue_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Textures/Medium_Blue_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Textures/Medium_Blue_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Textures/Medium_Blue_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Textures/Medium_Blue_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae/Textures/Medium_Blue_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Medium_Blue_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Textures/Medium_Blue_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Textures/Medium_Blue_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Textures/Medium_Blue_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Textures/Medium_Blue_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Textures/Medium_Blue_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_Local_Stars/Textures/Medium_Blue_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Medium_Blue_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Textures/Medium_Blue_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Textures/Medium_Blue_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Textures/Medium_Blue_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Textures/Medium_Blue_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Textures/Medium_Blue_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Blue_Nebulae_No_Stars/Textures/Medium_Blue_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Medium_Fiery_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Textures/Medium_Fiery_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Textures/Medium_Fiery_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Textures/Medium_Fiery_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Textures/Medium_Fiery_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Textures/Medium_Fiery_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae/Textures/Medium_Fiery_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Medium_Fiery_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Textures/Medium_Fiery_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Textures/Medium_Fiery_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Textures/Medium_Fiery_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Textures/Medium_Fiery_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Textures/Medium_Fiery_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_Local_Stars/Textures/Medium_Fiery_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Medium_Fiery_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Textures/Medium_Fiery_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Textures/Medium_Fiery_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Textures/Medium_Fiery_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Textures/Medium_Fiery_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Textures/Medium_Fiery_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Fiery_Nebulae_No_Stars/Textures/Medium_Fiery_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Medium_Mixed_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Textures/Medium_Mixed_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Textures/Medium_Mixed_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Textures/Medium_Mixed_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Textures/Medium_Mixed_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Textures/Medium_Mixed_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae/Textures/Medium_Mixed_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Medium_Mixed_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Textures/Medium_Mixed_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Textures/Medium_Mixed_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Textures/Medium_Mixed_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Textures/Medium_Mixed_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Textures/Medium_Mixed_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_Local_Stars/Textures/Medium_Mixed_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Medium_Mixed_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Textures/Medium_Mixed_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Textures/Medium_Mixed_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Textures/Medium_Mixed_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Textures/Medium_Mixed_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Textures/Medium_Mixed_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Mixed_Nebulae_No_Stars/Textures/Medium_Mixed_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Medium_Monochrome_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Textures/Medium_Monochrome_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Textures/Medium_Monochrome_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Textures/Medium_Monochrome_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Textures/Medium_Monochrome_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Textures/Medium_Monochrome_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae/Textures/Medium_Monochrome_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Medium_Monochrome_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Textures/Medium_Monochrome_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Textures/Medium_Monochrome_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Textures/Medium_Monochrome_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Textures/Medium_Monochrome_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Textures/Medium_Monochrome_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_Local_Stars/Textures/Medium_Monochrome_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Medium_Monochrome_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Textures/Medium_Monochrome_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Textures/Medium_Monochrome_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Textures/Medium_Monochrome_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Textures/Medium_Monochrome_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Textures/Medium_Monochrome_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Medium_Monochrome_Nebulae_No_Stars/Textures/Medium_Monochrome_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Rich_Blue_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Textures/Rich_Blue_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Textures/Rich_Blue_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Textures/Rich_Blue_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Textures/Rich_Blue_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Textures/Rich_Blue_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae/Textures/Rich_Blue_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Rich_Blue_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Textures/Rich_Blue_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Textures/Rich_Blue_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Textures/Rich_Blue_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Textures/Rich_Blue_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Textures/Rich_Blue_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_Local_Stars/Textures/Rich_Blue_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Rich_Blue_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Textures/Rich_Blue_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Textures/Rich_Blue_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Textures/Rich_Blue_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Textures/Rich_Blue_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Textures/Rich_Blue_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Blue_Nebulae_No_Stars/Textures/Rich_Blue_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Rich_Fiery_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Textures/Rich_Fiery_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Textures/Rich_Fiery_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Textures/Rich_Fiery_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Textures/Rich_Fiery_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Textures/Rich_Fiery_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae/Textures/Rich_Fiery_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Rich_Fiery_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Textures/Rich_Fiery_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Textures/Rich_Fiery_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Textures/Rich_Fiery_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Textures/Rich_Fiery_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Textures/Rich_Fiery_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_Local_Stars/Textures/Rich_Fiery_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Rich_Fiery_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Textures/Rich_Fiery_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Textures/Rich_Fiery_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Textures/Rich_Fiery_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Textures/Rich_Fiery_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Textures/Rich_Fiery_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Fiery_Nebulae_No_Stars/Textures/Rich_Fiery_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Rich_Mixed_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Textures/Rich_Mixed_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Textures/Rich_Mixed_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Textures/Rich_Mixed_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Textures/Rich_Mixed_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Textures/Rich_Mixed_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae/Textures/Rich_Mixed_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Rich_Mixed_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Textures/back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Textures/bottom.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Textures/front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Textures/left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Textures/right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_Local_Stars/Textures/top.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Rich_Mixed_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Textures/Rich_Mixed_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Textures/Rich_Mixed_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Textures/Rich_Mixed_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Textures/Rich_Mixed_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Textures/Rich_Mixed_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Mixed_Nebulae_No_Stars/Textures/Rich_Mixed_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Rich_Monochrome_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Textures/Rich_Monochrome_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Textures/Rich_Monochrome_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Textures/Rich_Monochrome_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Textures/Rich_Monochrome_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Textures/Rich_Monochrome_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae/Textures/Rich_Monochrome_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Rich_Monochrome_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Textures/Rich_Monochrome_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Textures/Rich_Monochrome_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Textures/Rich_Monochrome_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Textures/Rich_Monochrome_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Textures/Rich_Monochrome_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_Local_Stars/Textures/Rich_Monochrome_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Rich_Monochrome_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Textures/Rich_Monochrome_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Textures/Rich_Monochrome_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Textures/Rich_Monochrome_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Textures/Rich_Monochrome_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Textures/Rich_Monochrome_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Rich_Monochrome_Nebulae_No_Stars/Textures/Rich_Monochrome_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Sparse_Blue_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Textures/Sparse_Blue_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Textures/Sparse_Blue_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Textures/Sparse_Blue_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Textures/Sparse_Blue_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Textures/Sparse_Blue_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae/Textures/Sparse_Blue_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Sparse_Blue_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Textures/Sparse_Blue_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Textures/Sparse_Blue_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Textures/Sparse_Blue_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Textures/Sparse_Blue_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Textures/Sparse_Blue_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_Local_Stars/Textures/Sparse_Blue_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Sparse_Blue_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Textures/Sparse_Blue_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Textures/Sparse_Blue_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Textures/Sparse_Blue_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Textures/Sparse_Blue_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Textures/Sparse_Blue_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Blue_Nebulae_No_Stars/Textures/Sparse_Blue_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Sparse_Fiery_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Textures/Sparse_Fiery_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Textures/Sparse_Fiery_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Textures/Sparse_Fiery_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Textures/Sparse_Fiery_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Textures/Sparse_Fiery_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae/Textures/Sparse_Fiery_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Sparse_Fiery_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Textures/Sparse_Fiery_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Textures/Sparse_Fiery_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Textures/Sparse_Fiery_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Textures/Sparse_Fiery_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Textures/Sparse_Fiery_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_Local_Stars/Textures/Sparse_Fiery_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Sparse_Fiery_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Textures/Sparse_Fiery_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Textures/Sparse_Fiery_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Textures/Sparse_Fiery_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Textures/Sparse_Fiery_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Textures/Sparse_Fiery_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Fiery_Nebulae_No_Stars/Textures/Sparse_Fiery_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Sparse_Mixed_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Textures/Sparse_Mixed_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Textures/Sparse_Mixed_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Textures/Sparse_Mixed_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Textures/Sparse_Mixed_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Textures/Sparse_Mixed_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae/Textures/Sparse_Mixed_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Sparse_Mixed_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Textures/Sparse_Mixed_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Textures/Sparse_Mixed_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Textures/Sparse_Mixed_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Textures/Sparse_Mixed_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Textures/Sparse_Mixed_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_Local_Stars/Textures/Sparse_Mixed_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Sparse_Mixed_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Textures/Sparse_Mixed_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Textures/Sparse_Mixed_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Textures/Sparse_Mixed_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Textures/Sparse_Mixed_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Textures/Sparse_Mixed_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Mixed_Nebulae_No_Stars/Textures/Sparse_Mixed_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Sparse_Monochrome_Nebulae_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Textures/Sparse_Monochrome_Nebulae_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Textures/Sparse_Monochrome_Nebulae_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Textures/Sparse_Monochrome_Nebulae_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Textures/Sparse_Monochrome_Nebulae_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Textures/Sparse_Monochrome_Nebulae_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae/Textures/Sparse_Monochrome_Nebulae_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Sparse_Monochrome_Nebulae_Local_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Textures/Sparse_Monochrome_Nebulae_Local_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Textures/Sparse_Monochrome_Nebulae_Local_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Textures/Sparse_Monochrome_Nebulae_Local_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Textures/Sparse_Monochrome_Nebulae_Local_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Textures/Sparse_Monochrome_Nebulae_Local_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_Local_Stars/Textures/Sparse_Monochrome_Nebulae_Local_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Sparse_Monochrome_Nebulae_No_Stars_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Textures/Sparse_Monochrome_Nebulae_No_Stars_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Textures/Sparse_Monochrome_Nebulae_No_Stars_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Textures/Sparse_Monochrome_Nebulae_No_Stars_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Textures/Sparse_Monochrome_Nebulae_No_Stars_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Textures/Sparse_Monochrome_Nebulae_No_Stars_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Sparse_Monochrome_Nebulae_No_Stars/Textures/Sparse_Monochrome_Nebulae_No_Stars_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Starfield_And_Haze_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Textures/Starfield_And_Haze_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Textures/Starfield_And_Haze_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Textures/Starfield_And_Haze_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Textures/Starfield_And_Haze_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Textures/Starfield_And_Haze_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze/Textures/Starfield_And_Haze_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Starfield_And_Haze_Monochrome_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Textures/Starfield_And_Haze_Monochrome_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Textures/Starfield_And_Haze_Monochrome_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Textures/Starfield_And_Haze_Monochrome_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Textures/Starfield_And_Haze_Monochrome_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Textures/Starfield_And_Haze_Monochrome_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_And_Haze_Monochrome/Textures/Starfield_And_Haze_Monochrome_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Starfield_Only_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Textures/Starfield_Only_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Textures/Starfield_Only_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Textures/Starfield_Only_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Textures/Starfield_Only_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Textures/Starfield_Only_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only/Textures/Starfield_Only_up.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Starfield_Only_Monochrome_Skybox.mat \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Textures/Starfield_Only_Monochrome_back.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Textures/Starfield_Only_Monochrome_down.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Textures/Starfield_Only_Monochrome_front.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Textures/Starfield_Only_Monochrome_left.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Textures/Starfield_Only_Monochrome_right.png \
    Assets/Textures/4K_Space_Skyboxes/Materials/Starfield_Only_Monochrome/Textures/Starfield_Only_Monochrome_up.png \
    Assets/Textures/AncientPolygonMasonryScoria_basecolor.png \
    Assets/Textures/DefaultWhite.bmp \
    Assets/Textures/Directx9.png \
    Assets/Textures/Mona_Lisa.jpg \
    Assets/Textures/Starfield_And_Haze/Starfield_And_Haze_Skybox.mat \
    Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_back.png \
    Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_down.png \
    Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_front.png \
    Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_left.png \
    Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_right.png \
    Assets/Textures/Starfield_And_Haze/Textures/Starfield_And_Haze_up.png \
    Assets/Textures/Stone.png \
    Assets/Textures/awesomeface.png \
    Assets/Textures/container.jpg \
    Assets/Textures/container2.png \
    Assets/Textures/container2_specular.png \
    Assets/Textures/earth.dds \
    Assets/Textures/glass.png \
    Assets/Textures/grace-new.hdr \
    Assets/Textures/grace-new.hdr.txt \
    Assets/Textures/grass.png \
    Assets/Textures/planks.jpg \
    Assets/Textures/planks2.jpg \
    Assets/Textures/planks3.jpg \
    Assets/Textures/skybox/back.jpg \
    Assets/Textures/skybox/bottom.jpg \
    Assets/Textures/skybox/front.jpg \
    Assets/Textures/skybox/left.jpg \
    Assets/Textures/skybox/right.jpg \
    Assets/Textures/skybox/top.jpg \
    Assets/Textures/skybox2.png \
    Assets/Textures/wall.jpg \
    Include/glm/out/build/x64-Debug (default)/.cmake/api/v1/query/client-MicrosoftVS/query.json \
    Include/glm/out/build/x64-Debug (default)/.cmake/api/v1/reply/cache-v2-d553a5b5a87bb58ae14d.json \
    Include/glm/out/build/x64-Debug (default)/.cmake/api/v1/reply/cmakeFiles-v1-a227c4214d890b83c525.json \
    Include/glm/out/build/x64-Debug (default)/.cmake/api/v1/reply/codemodel-v2-98c925ab97f0409fa339.json \
    Include/glm/out/build/x64-Debug (default)/.cmake/api/v1/reply/index-2021-09-29T17-28-54-0754.json \
    Include/glm/out/build/x64-Debug (default)/CMakeCache.txt \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CMakeCCompiler.cmake \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CMakeCXXCompiler.cmake \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CMakeDetermineCompilerABI_C.bin \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CMakeDetermineCompilerABI_CXX.bin \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CMakeRCCompiler.cmake \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CMakeSystem.cmake \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CompilerIdC/CMakeCCompilerId.exe \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/3.18.20081302-MSVC_2/CompilerIdCXX/CMakeCXXCompilerId.exe \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/CMakeOutput.log \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/TargetDirectories.txt \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/cmake.check_cache \
    Include/glm/out/build/x64-Debug (default)/CMakeFiles/rules.ninja \
    Include/glm/out/build/x64-Debug (default)/Testing/Temporary/LastTest.log \
    Include/glm/out/build/x64-Debug (default)/VSInheritEnvironments.txt \
    Include/glm/out/build/x64-Debug (default)/build.ninja \
    Include/glm/out/build/x64-Debug (default)/cmake_install.cmake \
    plainshader.frag \
    plainshader.vert


INCLUDEPATH += $$PWD/Libs
DEPENDPATH += $$PWD/Libs


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Libs/ -lassimp-vc140-mt
else:unix: LIBS += -L$$PWD/Libs/ -lassimp-vc140-mt

INCLUDEPATH += $$PWD/Libs
DEPENDPATH += $$PWD/Libs

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Libs/libassimp-vc140-mt.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/Libs/assimp-vc140-mt.lib
else:unix: PRE_TARGETDEPS += $$PWD/Libs/libassimp-vc140-mt.a

unix|win32: LIBS += -L$$PWD/Libs/ -lassimp-vc140-mt

INCLUDEPATH += $$PWD/Libs
DEPENDPATH += $$PWD/Libs
