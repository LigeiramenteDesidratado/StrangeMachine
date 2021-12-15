#include "smMesh.h"
#include "util/common.h"

bool obj_loader_load(mesh_s **meshes, const char* path) {

  FILE *fp;
  fp = fopen(path, "r");
  if (fp == NULL) {
    log_error("[%s] failed to open obj file", path);
    return false;
  }

  size_t objects = 0;
  size_t iv_len = 0, iu_len = 0, in_len = 0;
  size_t tmp_v_len = 0, tmp_uv_len = 0, tmp_n_len = 0;

  char* _line = NULL, *line = NULL;
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

  // move the file pointer to the beggining
  fseek(fp, 0, SEEK_SET);
  vec3 *tmp_v = NULL;
  arrsetcap(tmp_v, tmp_v_len);

  vec2 *tmp_uv = NULL;
  arrsetcap(tmp_uv, tmp_uv_len);

  vec3 *tmp_n = NULL;
  arrsetcap(tmp_n, tmp_n_len);

  size_t *iv = NULL, *iu = NULL, *in = NULL;
  arrsetcap(iv, iv_len);
  arrsetcap(iu, iu_len);
  arrsetcap(in, in_len);

  arrsetlen((*meshes), objects);

  for (size_t i = 0; i < arrlenu((*meshes)); ++i) {
    mesh_s *mesh = &(*meshes)[i];
    (*mesh) = mesh_new();

    arrsetcap(mesh->vertex.positions, iv_len);
    // mesh->vertex.positions_len = iv_len;
    // mesh->vertex.positions = malloc(sizeof(vec3) * iv_len);

    arrsetcap(mesh->vertex.tex_coords, iu_len);
    // mesh->vertex.tex_coords_len = iu_len;
    // mesh->vertex.tex_coords = malloc(sizeof(vec2) * iu_len);

    arrsetcap(mesh->vertex.normals, in_len);
    // mesh->vertex.normals_len = in_len;
    // mesh->vertex.normals = malloc(sizeof(vec3) * in_len);
  }

  while (1) {

    char lineHeader[128];
    // read the first word of the line
    int res = fscanf(fp, "%s", lineHeader);
    if (res == EOF)
      break;

    if (strcmp(lineHeader, "v") == 0) {
      vec3 v = {0};
      fscanf(fp, "%f %f %f\n", &v.x, &v.y, &v.z);
      arrput(tmp_v, v);
    } else if (strcmp(lineHeader, "vt") == 0) {
      vec2 uv;
      fscanf(fp, "%f %f\n", &uv.x, &uv.y);
      arrput(tmp_uv, uv);
    } else if (strcmp(lineHeader, "vn") == 0) {
      vec3 n;
      fscanf(fp, "%f %f %f\n", &n.x, &n.y, &n.z);
      arrput(tmp_n, n);
    } else if (strcmp(lineHeader, "f") == 0) {
      uint32_t vi[3], ui[3], ni[3];

      int matches =
          fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vi[0], &ui[0], &ni[0],
                 &vi[1], &ui[1], &ni[1], &vi[2], &ui[2], &ni[2]);

      assert(matches == 9 &&
             "should export your model with 'Triangulate faces' checked" &&
             "make sure that vertex, uv and normals are being exported");

      arrput(iv, vi[0]);
      arrput(iv, vi[1]);
      arrput(iv, vi[2]);

      arrput(iu, ui[0]);
      arrput(iu, ui[1]);
      arrput(iu, ui[2]);

      arrput(in, ni[0]);
      arrput(in, ni[1]);
      arrput(in, ni[2]);
    } else { // Probably a comment, eat up the rest of the line
      char stupidBuffer[512];
      fgets(stupidBuffer, 512, fp);
    }
  }

  for (size_t i = 0; i < arrlenu((*meshes)); ++i) {

    mesh_s *mesh = &(*meshes)[i];

    for (uint32_t j = 0; j < iv_len; ++j) {
      size_t vertex_index = iv[j];
      vec3 vertex = tmp_v[vertex_index - 1];
      arrput(mesh->vertex.positions, vertex);
    }

    for (uint32_t j = 0; j < in_len; ++j) {
      size_t nv_index = in[j];
      vec3 normal = tmp_n[nv_index - 1];
      arrput(mesh->vertex.normals, normal);
    }

    for (uint32_t j = 0; j < iu_len; ++j) {
      size_t uv_index = iu[j];
      vec2 uv = tmp_uv[uv_index - 1];
      /* uv.y = 1.0f - uv.y; */
      arrput(mesh->vertex.tex_coords, uv);
    }
  }

  fclose(fp);
  arrfree(tmp_uv);
  arrfree(tmp_v);
  arrfree(tmp_n);

  arrfree(iv);
  arrfree(iu);
  arrfree(in);

  return true;
}
