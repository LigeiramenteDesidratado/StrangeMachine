#include "util/common.h"

#include "data/array.h"

#include "smMesh.h"

bool obj_loader_load(mesh_s **meshes, const char *path) {

  FILE *fp;
  fp = fopen(path, "r");
  if (fp == NULL) {
    SM_LOG_ERROR("[%s] failed to open obj file", path);
    return false;
  }

  size_t objects = 0;
  size_t iv_len = 0, iu_len = 0, in_len = 0;
  size_t tmp_v_len = 0, tmp_uv_len = 0, tmp_n_len = 0;

  char *_line = NULL, *line = NULL;
  size_t len = 0;
  ssize_t read;
  while ((read = getline(&_line, &len, fp)) != -1) {

    // preserve original pointer
    line = _line;

    if (*line == 'v') {
      // move to the next char
      line++;
      if (*line == ' ') {
        tmp_v_len++;
      } else if (*line == 't') {
        tmp_uv_len++;
      } else if (*line == 'n') {
        tmp_n_len++;
      }
    } else if (*line == 'f') {
      iv_len += 3;
      iu_len += 3;
      in_len += 3;
    } else if (*line == 'o') {
      objects++;
    }
  }

  // move the file pointer to the begining
  fseek(fp, 0, SEEK_SET);
  vec3 *tmp_v = NULL;
  SM_ARRAY_SET_CAPACITY(tmp_v, tmp_v_len);

  vec2 *tmp_uv = NULL;
  SM_ARRAY_SET_CAPACITY(tmp_uv, tmp_uv_len);

  vec3 *tmp_n = NULL;
  SM_ARRAY_SET_CAPACITY(tmp_n, tmp_n_len);

  size_t *iv = NULL, *iu = NULL, *in = NULL;
  SM_ARRAY_SET_CAPACITY(iv, iv_len);
  SM_ARRAY_SET_CAPACITY(iu, iu_len);
  SM_ARRAY_SET_CAPACITY(in, in_len);

  SM_ARRAY_SET_SIZE((*meshes), objects);

  for (size_t i = 0; i < SM_ARRAY_SIZE((*meshes)); ++i) {
    mesh_s *mesh = &(*meshes)[i];
    (*mesh) = mesh_new();

    SM_ARRAY_SET_CAPACITY(mesh->vertex.positions, iv_len);
    SM_ARRAY_SET_CAPACITY(mesh->vertex.tex_coords, iu_len);
    SM_ARRAY_SET_CAPACITY(mesh->vertex.normals, in_len);
  }

  while (1) {

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(fp, "%s", lineHeader);
    if (res == EOF)
      break;

    if (strcmp(lineHeader, "v") == 0) {
      vec3 v = {0};
      fscanf(fp, "%f %f %f\n", &v[0], &v[1], &v[2]);
      SM_ARRAY_PUSH(tmp_v, v);
    } else if (strcmp(lineHeader, "vt") == 0) {
      vec2 uv;
      fscanf(fp, "%f %f\n", &uv[0], &uv[1]);
      SM_ARRAY_PUSH(tmp_uv, uv);
    } else if (strcmp(lineHeader, "vn") == 0) {
      vec3 n;
      fscanf(fp, "%f %f %f\n", &n[0], &n[1], &n[2]);
      SM_ARRAY_PUSH(tmp_n, n);
    } else if (strcmp(lineHeader, "f") == 0) {
      uint32_t vi[3], ui[3], ni[3];

      int matches = fscanf(fp, "%u/%u/%u %u/%u/%u %u/%u/%u\n", &vi[0], &ui[0], &ni[0], &vi[1], &ui[1], &ni[1], &vi[2],
                           &ui[2], &ni[2]);

      SM_ASSERT(matches == 9 && "should export your model with 'Triangulate faces' checked" &&
                "make sure that vertex, uv and normals are being exported");

      SM_ARRAY_PUSH(iv, vi[0]);
      SM_ARRAY_PUSH(iv, vi[1]);
      SM_ARRAY_PUSH(iv, vi[2]);

      SM_ARRAY_PUSH(iu, ui[0]);
      SM_ARRAY_PUSH(iu, ui[1]);
      SM_ARRAY_PUSH(iu, ui[2]);

      SM_ARRAY_PUSH(in, ni[0]);
      SM_ARRAY_PUSH(in, ni[1]);
      SM_ARRAY_PUSH(in, ni[2]);
    } else { // Probably a comment, eat up the rest of the line
      char stupidBuffer[512];
      fgets(stupidBuffer, 512, fp);
    }
  }

  for (size_t i = 0; i < SM_ARRAY_SIZE((*meshes)); ++i) {

    mesh_s *mesh = &(*meshes)[i];

    for (uint32_t j = 0; j < iv_len; ++j) {
      size_t vertex_index = iv[j];
      vec3 vertex;
      glm_vec3_copy(tmp_v[vertex_index - 1], vertex);
      SM_ARRAY_PUSH(mesh->vertex.positions, vertex);
    }

    for (uint32_t j = 0; j < in_len; ++j) {
      size_t nv_index = in[j];
      vec3 normal;
      glm_vec3_copy(tmp_n[nv_index - 1], normal);
      SM_ARRAY_PUSH(mesh->vertex.normals, normal);
    }

    for (uint32_t j = 0; j < iu_len; ++j) {
      size_t uv_index = iu[j];
      vec2 uv;
      glm_vec2_copy(tmp_uv[uv_index - 1], uv);
      /* uv.y = 1.0f - uv.y; */
      SM_ARRAY_PUSH(mesh->vertex.tex_coords, uv);
    }
  }

  fclose(fp);
  SM_ARRAY_DTOR(tmp_uv);
  SM_ARRAY_DTOR(tmp_v);
  SM_ARRAY_DTOR(tmp_n);

  SM_ARRAY_DTOR(iv);
  SM_ARRAY_DTOR(iu);
  SM_ARRAY_DTOR(in);

  return true;
}
