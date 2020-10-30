#if !defined(_VIEW_H)
#define _VIEW_H

#include <main.h>
#include "data.h"

#define EDJ_FILE "edje/main.edj"
#define GRP_MAIN "main"

Eina_Bool view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_conformant_without_indicator(Evas_Object *win);
void view_init_tennis_scores_theme(char *theme);
void view_fini_tennis_scores_theme(char *theme);
void view_destroy(void);
void view_create_tennis_scores_layout(char *file_path, char *group_name);
Evas_Object *view_create_layout_for_conformant(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data);
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data);
Evas_Object *view_create_layout_by_theme(Evas_Object *parent, const char *class_name, const char *group_name, const char *style);
void view_create_my_points_label(void);
void view_create_op_points_label(void);
void view_create_my_scores_label(void);
void view_create_op_scores_label(void);
void view_create_scores_button(button_score *btn_score, Evas_Smart_Cb clicked_cb);
void view_display_my_scores(int points, int game, int set);
void view_display_op_scores(int points, int game, int set);

#endif
