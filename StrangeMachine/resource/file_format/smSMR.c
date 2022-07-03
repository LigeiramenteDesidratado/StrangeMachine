#include "smpch.h"

#include "core/smCore.h"
#include "resource/file_format/smSMR.h"

smr_file_header_s smr_file_header_new(u64 resource_type) {

  smr_file_header_s header = {
      .signature = SMR_MAGIC_NUMBER, .version = SMR_VERSION_NUMBER, .resouce_type = resource_type};
  return header;
}

b8 smr_write_header(smr_file_header_s *header, sm_file_handle_s *file) {

  SM_ASSERT(file);

  if (header->signature != SMR_MAGIC_NUMBER) {
    SM_LOG_ERROR("invalid signature: %x", header->signature);
    return false;
  }

  if (header->version > SMR_VERSION_NUMBER) {
    SM_LOG_ERROR("invalid version: %x", header->version);
    return false;
  }

  if (header->resouce_type == SMR_RESOURCE_NONE) {
    SM_LOG_ERROR("invalid resource type: %lx", header->resouce_type);
    return false;
  }

  /* if (sm_filesystem_size(file) != 0) { */
  /*   SM_LOG_ERROR("file is not empty"); */
  /*   return false; */
  /* } */

  if (!sm_filesystem_write_bytes(file, header, sizeof(smr_file_header_s))) {
    SM_LOG_ERROR("[s] failed to write header");
    return false;
  }

  return true;
}

b8 smr_read_header(smr_file_header_s *header, sm_file_handle_s *file) {

  SM_ASSERT(file);

  if (sm_filesystem_size(file) < sizeof(smr_file_header_s)) {
    SM_LOG_ERROR("file is too small");
    return false;
  }

  if (!sm_filesystem_read_bytes(file, header, sizeof(smr_file_header_s))) {
    SM_LOG_ERROR("[s] failed to read header");
    return false;
  }

  if (header->signature != SMR_MAGIC_NUMBER) {
    SM_LOG_ERROR("invalid signature: %x", header->signature);
    return false;
  }

  if (header->version != SMR_VERSION_NUMBER) {
    SM_LOG_ERROR("invalid version: %x", header->version);
    return false;
  }

  if (header->resouce_type == SMR_RESOURCE_NONE) {
    SM_LOG_ERROR("invalid resource type: %lx", header->resouce_type);
    return false;
  }

  return true;
}

// b8 smr_write_mesh(sm_string path, sm_mesh_s *mesh) {
//
//   sm_file_handle_s f;
//   if (!sm_filesystem_open(path, SM_FILE_MODE_WRITE, true, &f)) {
//     SM_LOG_ERROR("[%s] failed to open file", path.str);
//     return false;
//   }
//
//   smr_file_header header = sm_file_header_new(SMR_RESOURCE_TYPE_MODEL);
//   if (!sm_filesystem_write_bytes(&f, &header, sizeof(smr_file_header))) {
//     SM_LOG_ERROR("[%s] failed to write header", path.str);
//     return false;
//   }
//
//   size_t vertices_length = SM_ARRAY_LEN(mesh->vertices);
//   if (!sm_filesystem_write_bytes(&f, &vertices_length, sizeof(size_t))) {
//     SM_LOG_ERROR("[%s] failed to write the length of vertices", path.str);
//     return false;
//   }
//
//   for (size_t i = 0; i < SM_ARRAY_LEN(mesh->vertices); i++) {
//
//     sm_vertex_s *vertices = &mesh->vertices[i];
//     sm_string name = mesh->name[i];
//
//     SM_LOG_TRACE("'%s' writing mesh %zu", name.str, i);
//     size_t len = sm_string_len(name);
//     size_t size = sizeof(char);
//     SM_LOG_TRACE("writting %zu:%zu string", len, size);
//     if (!sm_filesystem_write_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the length the string name", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the size of the string name", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, name.str, len * size)) {
//       SM_LOG_ERROR("[%s] failed to write string name", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_write_bytes(&f, sm_transform_zero().data, sizeof(sm_mat4))) {
//       SM_LOG_ERROR("[%s] failed to write transform", path.str);
//       return false;
//     }
//
//     len = SM_ARRAY_LEN(vertices->positions);
//     size = sizeof(*vertices->positions);
//     SM_LOG_TRACE("writing %zu:%zu positions", len, size);
//     if (!sm_filesystem_write_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the length of positions", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the size of positions", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, vertices->positions, len * size)) {
//       SM_LOG_ERROR("[%s] failed to write positions", path.str);
//       return false;
//     }
//
//     len = SM_ARRAY_LEN(vertices->uvs);
//     size = sizeof(*vertices->uvs);
//     SM_LOG_TRACE("writing %zu:%zu uvs", len, size);
//     if (!sm_filesystem_write_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the length of uvs", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the size of uvs", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, vertices->uvs, len * size)) {
//       SM_LOG_ERROR("[%s] failed to write uvs", path.str);
//       return false;
//     }
//
//     len = SM_ARRAY_LEN(vertices->colors);
//     size = sizeof(*vertices->colors);
//     SM_LOG_TRACE("writing %zu:%zu colors", len, size);
//     if (!sm_filesystem_write_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the length of colors", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the size of colors", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, vertices->colors, len * size)) {
//       SM_LOG_ERROR("[%s] failed to write colors", path.str);
//       return false;
//     }
//
//     len = SM_ARRAY_LEN(vertices->normals);
//     size = sizeof(*vertices->normals);
//     SM_LOG_TRACE("writing %zu:%zu normals", len, size);
//     if (!sm_filesystem_write_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the length of normals", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the size of normals", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, vertices->normals, len * size)) {
//       SM_LOG_ERROR("[%s] failed to write normals", path.str);
//       return false;
//     }
//
//     len = SM_ARRAY_LEN(vertices->indices);
//     size = sizeof(*vertices->indices);
//     SM_LOG_TRACE("writing %zu:%zu indices", len, size);
//     if (!sm_filesystem_write_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the length of indices", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to write the size of indices", path.str);
//       return false;
//     }
//     if (!sm_filesystem_write_bytes(&f, vertices->indices, len * size)) {
//       SM_LOG_ERROR("[%s] failed to write indices", path.str);
//       return false;
//     }
//   }
//   if (!sm_filesystem_close(&f)) {
//     SM_LOG_ERROR("[%s] failed to close file", path.str);
//     return false;
//   }
//   return true;
// }
//
// b8 smr_read_mesh(sm_string path, sm_mesh_s *mesh) {
//
//   if (!sm_filesystem_exists(path)) {
//     SM_LOG_ERROR("[%s] does not exist", path.str);
//     return false;
//   }
//
//   sm_file_handle_s f;
//   if (!sm_filesystem_open(path, SM_FILE_MODE_READ, true, &f)) {
//     SM_LOG_ERROR("[%s] failed to open file", path.str);
//     return false;
//   }
//
//   if (sm_filesystem_size(&f) <= sizeof(smr_file_header)) {
//     printf("[%s] file is too small\n", path.str);
//     return false;
//   }
//
//   smr_file_header header;
//   if (!sm_filesystem_read_bytes(&f, &header, sizeof(smr_file_header))) {
//     SM_LOG_ERROR("[%s] failed to read header", path.str);
//     return false;
//   }
//
//   if (header.signature != SMR_MAGIC_NUMBER) {
//     SM_LOG_ERROR("[%s] invalid magic", path.str);
//     return false;
//   }
//
//   if (header.version > SMR_VERSION_NUMBER) {
//     SM_LOG_ERROR("[%s] invalid version", path.str);
//     return false;
//   }
//
//   if (header.resouce_type != SMR_RESOURCE_TYPE_MODEL) {
//     SM_LOG_ERROR("[%s] invalid resource type", path.str);
//     return false;
//   }
//
//   size_t vertices_length = 0;
//   if (!sm_filesystem_read_bytes(&f, &vertices_length, sizeof(size_t))) {
//     SM_LOG_ERROR("[%s] failed to read the length of the string", path.str);
//     return false;
//   }
//   SM_ARRAY_SET_CAP(mesh->vertices, vertices_length);
//   SM_ARRAY_SET_CAP(mesh->name, vertices_length);
//   SM_LOG_TRACE("reading %zu meshes", vertices_length);
//
//   while (vertices_length--) {
//     /* sm_vertex_s vertex = {0}; */
//     sm_string name;
//     size_t len = 0;
//     size_t size = 0;
//     if (!sm_filesystem_read_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the length of the string", path.str);
//       return false;
//     }
//     if (!sm_filesystem_read_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the size of the string", path.str);
//       return false;
//     }
//     SM_LOG_TRACE("reading %zu:%zu string", len, size);
//     char buf[len];
//     if (!sm_filesystem_read_bytes(&f, buf, len * size)) {
//       SM_LOG_ERROR("[%s] failed to read the string", path.str);
//       return false;
//     }
//     buf[len] = '\0';
//     name = sm_string_from(buf);
//
//     SM_LOG_TRACE("reading '%s'", name.str);
//
//     sm_mat4 transform;
//     if (!sm_filesystem_read_bytes(&f, &transform, sizeof(sm_mat4))) {
//       SM_LOG_ERROR("[%s] failed to read the transform", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the length of positions", path.str);
//       return false;
//     }
//     if (!sm_filesystem_read_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the size of positions", path.str);
//       return false;
//     }
//     SM_LOG_TRACE("reading %zu:%zu vertices", len, size);
//     SM_ARRAY_SET_LEN(mesh.positions, len);
//     if (!sm_filesystem_read_bytes(&f, mesh.positions, len * size)) {
//       SM_LOG_ERROR("[%s] failed to read vertices", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the length of uvs", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the size of uvs", path.str);
//       return false;
//     }
//
//     SM_LOG_TRACE("reading %zu:%zu uvs", len, size);
//
//     SM_ARRAY_SET_LEN(mesh.uvs, len);
//     if (!sm_filesystem_read_bytes(&f, mesh.uvs, len * size)) {
//       SM_LOG_ERROR("[%s] failed to read uvs", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the length of colors", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the size of colors", path.str);
//       return false;
//     }
//
//     SM_LOG_TRACE("reading %zu:%zu colors", len, size);
//     SM_ARRAY_SET_LEN(mesh.colors, len);
//     if (!sm_filesystem_read_bytes(&f, mesh.colors, len * size)) {
//       SM_LOG_ERROR("[%s] failed to read colors", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the length of normals", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the size of normals", path.str);
//       return false;
//     }
//
//     SM_LOG_TRACE("reading %zu:%zu normals", len, size);
//
//     SM_ARRAY_SET_LEN(mesh.normals, len);
//     if (!sm_filesystem_read_bytes(&f, mesh.normals, len * size)) {
//       SM_LOG_ERROR("[%s] failed to read normals", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &len, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the length of indices", path.str);
//       return false;
//     }
//
//     if (!sm_filesystem_read_bytes(&f, &size, sizeof(size_t))) {
//       SM_LOG_ERROR("[%s] failed to read the size of indices", path.str);
//       return false;
//     }
//
//     SM_LOG_TRACE("reading %zu:%zu indices", len, size);
//
//     SM_ARRAY_SET_LEN(mesh.indices, len);
//     if (!sm_filesystem_read_bytes(&f, mesh.indices, len * size)) {
//       SM_LOG_ERROR("[%s] failed to read indices", path.str);
//       return false;
//     }
//
//     mesh->name = name;
//     SM_ARRAY_PUSH(mesh, mesh);
//   }
//   return true;
// }
