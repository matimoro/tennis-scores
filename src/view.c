#include <tizen.h>
#include <Elementary.h>
#include <efl_extension.h>
#include <app.h>
#include <dlog.h>
#include <main.h>
#include "view.h"

/**
 * Variables regarding with main view.
 */
static struct view_info {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *layout;
	Evas_Object *my_points_label;
	Evas_Object *op_points_label;
	Evas_Object *my_scores_label[3];
	Evas_Object *op_scores_label[3];
	Elm_Theme *theme;
} s_info = {
	.win = NULL,
	.conform = NULL,
	.layout = NULL,
	.my_points_label = NULL,
	.op_points_label = NULL,
	.my_scores_label = NULL,
	.op_scores_label = NULL,
	.theme = NULL,
};

static void _win_delete_request_cb(void *user_data, Evas_Object *obj, void *event_info);
static void _layout_back_cb(void *data, Evas_Object *obj, void *event_info);
static void _button_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _button_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

/**
 * @brief Create Essential Object window, conformant and layout.
 */
Eina_Bool view_create(void)
{
	/* Create window */
	s_info.win = view_create_win(PACKAGE);
	if (s_info.win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");
		return EINA_FALSE;
	}

	/* Create conformant */
	s_info.conform = view_create_conformant_without_indicator(s_info.win);
	if (s_info.conform == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a conformant");
		return EINA_FALSE;
	}

	/* Show window after main view is set up */
	evas_object_show(s_info.win);
	return EINA_TRUE;
}

/**
 * @brief Creates a basic window named package.
 * @param[in] pkg_name Name of the window
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	win = elm_win_util_standard_add(pkg_name, pkg_name);
	elm_win_conformant_set(win, EINA_TRUE);
	elm_win_autodel_set(win, EINA_TRUE);

	/* Rotation setting */
	if (elm_win_wm_rotation_supported_get(win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(win, "delete,request", _win_delete_request_cb, NULL);

	return win;
}

/**
 * @brief Creates a conformant without indicator for wearable app.
 * @param[in] win The object to which you want to set this conformant
 * Conformant is mandatory for base GUI to have proper size
 */
Evas_Object *view_create_conformant_without_indicator(Evas_Object *win)
{
	/*
	 * Conformant
	 * Create and initialize elm_conformant.
	 * elm_conformant is mandatory for base GUI to have proper size
	 * when indicator or virtual keypad is visible.
	 */
	Evas_Object *conform = NULL;

	if (win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "window is NULL.");
		return NULL;
	}

	conform = elm_conformant_add(win);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(win, conform);

	evas_object_show(conform);

	return conform;
}

/**
 * @brief Creates new theme using EDJ file.
 * @param[in] theme The path of EDJ file will be used for the theme
 */
void view_init_tennis_scores_theme(char *theme)
{
	/* Create new theme object */
	s_info.theme = elm_theme_new();
	if (s_info.theme == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create new theme");
		return;
	}
	elm_theme_ref_set(s_info.theme, NULL);
	/* Set the 'theme' to new theme object 's_info.theme' */
	elm_theme_extension_add(s_info.theme, theme);
}

/**
 * @brief Destroys new theme using EDJ file.
 * @param[in] theme The path of EDJ file have been used for the theme
 */
void view_fini_tennis_scores_theme(char *theme)
{
	if (s_info.theme == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "There's no theme");
		return;
	}
	elm_theme_extension_del(s_info.theme, theme);
	/* It should be free */
	elm_theme_free(s_info.theme);
	/* It should be set as NULL */
	s_info.theme = NULL;
}

/**
 * @brief Destroys window and free important data to finish this application if necessary.
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/**
 * @brief Creates a layout using group name of the EDJ file.
 * @param[in] file_path File path of the EDJ file will be used
 * @param[in] group_name Group name of the EDJ file will be used
 */
void view_create_tennis_scores_layout(char *file_path, char *group_name)
{
	/* Creates layout using EDJ file 'file_path' and in there certain group 'group_name' will be used */
	s_info.layout = view_create_layout_for_conformant(s_info.conform, file_path, group_name, _layout_back_cb, NULL);
	evas_object_show(s_info.layout);
}

/**
 * @brief Creates and sets a layout to conformant.
 * @param[in] parent Target conformant object
 * @param[in] file_path File path of EDJ will be used
 * @param[in] group_name Group name in EDJ you want to set to layout
 * @param[in] cb_function The function will be called when the back event is detected
 * @param[in] user_data The user data to be passed to the callback functions
 */
Evas_Object *view_create_layout_for_conformant(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	/* If there's no EDJ file, layout will use the default EDJ file given by EFL */
	if (file_path == NULL)
		layout = view_create_layout_by_theme(parent, "layout", "application", "default");
	else
		layout = view_create_layout(parent, file_path, group_name, cb_function, user_data);

	if (layout == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "layout is NULL.");
		return NULL;
	}

	elm_object_content_set(parent, layout);

	return layout;
}

/**
 * @brief Creates a layout with theme.
 * @param[in] parent Object to which you want to add this layout
 * @param[in] class_name The class of the group
 * @param[in] group_name Group name in EDJ that you want to set to layout
 * @param[in] style The style to use
 */
Evas_Object *view_create_layout_by_theme(Evas_Object *parent, const char *class_name, const char *group_name, const char *style)
{
	/*
	 * Layout
	 * Create and initialize elm_layout.
	 * view_create_layout_by_theme() is used to create layout by using pre-made edje file.
	 */
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, class_name, group_name, style);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	evas_object_show(layout);

	return layout;
}

/**
 * @brief Creates a layout to target parent object with edje file.
 * @param[in] parent The object to which you want to add this layout
 * @param[in] file_path File path of the EDJ file will be used
 * @param[in] group_name Name of group in EDJ you want to set to
 * @param[in] cb_function The function called when back event is detected
 * @param[in] user_data The user data to be passed to the callback function
 */
Evas_Object *view_create_layout(Evas_Object *parent, const char *file_path, const char *group_name, Eext_Event_Cb cb_function, void *user_data)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	/* Create layout by EDC(edje file) */
	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, file_path, group_name);

	/* Layout size setting */
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (cb_function)
		eext_object_event_callback_add(layout, EEXT_CALLBACK_BACK, cb_function, user_data);

	evas_object_show(layout);

	return layout;
}

/**
 * @brief Creates a my points label.
 */
void view_create_my_points_label(void)
{
	s_info.my_points_label = elm_label_add(s_info.layout);
	if (s_info.my_points_label == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create label");
		return;
	}

	/* This decide how many texts will be displayed in a line */
	elm_label_wrap_width_set(s_info.my_points_label, 185);
	/* Not ellipsis */
	elm_label_ellipsis_set(s_info.my_points_label, EINA_FALSE);
	evas_object_resize(s_info.my_points_label, 90, 90);
	/* Display '0' when no formula is there */
	char styled_text[1024];
	snprintf(styled_text, sizeof(styled_text), "<font_size=50><align=right>%s</align></font_size>", "0");
	elm_object_text_set(s_info.my_points_label, styled_text);
	/* Set label object to the part named "label" in EDJ file */
	elm_object_part_content_set(s_info.layout, "my_points_label", s_info.my_points_label);
	evas_object_show(s_info.my_points_label);

}

/**
 * @brief Creates a op points label.
 */
void view_create_op_points_label(void)
{
	s_info.op_points_label = elm_label_add(s_info.layout);
	if (s_info.op_points_label == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create label");
		return;
	}

	/* This decide how many texts will be displayed in a line */
	elm_label_wrap_width_set(s_info.op_points_label, 185);
	/* Not ellipsis */
	elm_label_ellipsis_set(s_info.op_points_label, EINA_FALSE);
	evas_object_resize(s_info.op_points_label, 90, 90);
	/* Display '0' when no formula is there */
	char styled_text[1024];
	snprintf(styled_text, sizeof(styled_text), "<font_size=50><align=right>%s</align></font_size>", "0");
	elm_object_text_set(s_info.op_points_label, styled_text);
	/* Set label object to the part named "label" in EDJ file */
	elm_object_part_content_set(s_info.layout, "op_points_label", s_info.op_points_label);
	evas_object_show(s_info.op_points_label);

}

/**
 * @brief Creates a my scores label.
 */
void view_create_my_scores_label(void)
{
	int i;

	for (i = 0; i <= 3; i++) {
		s_info.my_scores_label[i] = elm_label_add(s_info.layout);
		if (s_info.my_scores_label[i] == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create label");
			return;
		}

		/* This decide how many texts will be displayed in a line */
		elm_label_wrap_width_set(s_info.my_scores_label[i], 185);
		/* Not ellipsis */
		elm_label_ellipsis_set(s_info.my_scores_label[i], EINA_FALSE);
		evas_object_resize(s_info.my_scores_label[i], 90, 90);
		/* Display '0' when no formula is there */
		char styled_text[1024];
		snprintf(styled_text, sizeof(styled_text), "<font_size=25><align=right>%s</align></font_size>", "0");
		elm_object_text_set(s_info.my_scores_label[i], styled_text);
		/* Set label object to the part named "label" in EDJ file */
		char label_name[1024];
		snprintf(label_name, sizeof(label_name), "my_scores_label_%d", i);
		elm_object_part_content_set(s_info.layout, label_name, s_info.my_scores_label[i]);
		evas_object_show(s_info.my_scores_label[i]);
	}
}

/**
 * @brief Creates a op scores label.
 */
void view_create_op_scores_label(void)
{
	int i;

	for (i = 0; i <= 3; i++) {
		s_info.op_scores_label[i] = elm_label_add(s_info.layout);
		if (s_info.op_scores_label[i] == NULL) {
			dlog_print(DLOG_ERROR, LOG_TAG, "Failed to create label");
			return;
		}

		/* This decide how many texts will be displayed in a line */
		elm_label_wrap_width_set(s_info.op_scores_label[i], 185);
		/* Not ellipsis */
		elm_label_ellipsis_set(s_info.op_scores_label[i], EINA_FALSE);
		evas_object_resize(s_info.op_scores_label[i], 90, 90);
		/* Display '0' when no formula is there */
		char styled_text[1024];
		snprintf(styled_text, sizeof(styled_text), "<font_size=25><align=right>%s</align></font_size>", "0");
		elm_object_text_set(s_info.op_scores_label[i], styled_text);
		/* Set label object to the part named "label" in EDJ file */
		char label_name[1024];
		snprintf(label_name, sizeof(label_name), "op_scores_label_%d", i);
		elm_object_part_content_set(s_info.layout, label_name, s_info.op_scores_label[i]);
		evas_object_show(s_info.op_scores_label[i]);
	}
}

/**
 * @brief Creates each button for calculator.
 * @param[in] btn_info Information related to the button
 * @param[in] clicked_cb Function will be called when the button is clicked
 */
void view_create_scores_button(button_score *btn_score, Evas_Smart_Cb clicked_cb)
{
	/* Set the variables pre-declared to each button */
	dlog_print(DLOG_INFO, LOG_TAG, "name: %s", btn_score->button_name);

	btn_score->button = elm_button_add(s_info.layout);
	/* This is possible by 'theme_set() in main.c' using EDJ file
	 * This make the button transparent
	 */
	elm_object_style_set(btn_score->button, "transparent");
	/* EVAS_HINT_EXPAND make the button full size of the given position */
	evas_object_size_hint_weight_set(btn_score->button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(s_info.layout, btn_score->button_name, btn_score->button);
	/* Set the function will be called on click */
	evas_object_smart_callback_add(btn_score->button, "clicked", clicked_cb, btn_score);
	/* Set the function will be called on press */
	evas_object_event_callback_add(btn_score->button, EVAS_CALLBACK_MOUSE_DOWN, _button_down_cb, btn_score);
	/* Set the function will be called on up */
	evas_object_event_callback_add(btn_score->button, EVAS_CALLBACK_MOUSE_UP, _button_up_cb, btn_score);
	evas_object_show(btn_score->button);
}

/**
 * @brief Displays the my score on the screen.
 */
void view_display_my_scores(int points, int game, int set)
{
	char styled_text[1024];
	const char *my_score_str;

	switch (points) {
	case POINT_LOVE:
		my_score_str = "0";
		break;
	case POINT_15:
		my_score_str = "15";
		break;
	case POINT_30:
		my_score_str = "30";
		break;
	case POINT_40:
		my_score_str = "40";
		break;
	default:
		break;
	}

	dlog_print(DLOG_INFO, LOG_TAG, "my displayed points: %s", my_score_str);
	snprintf(styled_text, sizeof(styled_text), "<font_size=50><align=right>%s</align></font_size>", my_score_str);
	elm_object_text_set(s_info.my_points_label, styled_text);

	dlog_print(DLOG_INFO, LOG_TAG, "my displayed game: %d", game);
	snprintf(styled_text, sizeof(styled_text), "<font_size=25><align=right>%d</align></font_size>", game);
	elm_object_text_set(s_info.my_scores_label[1], styled_text);
}


/**
 * @brief Displays the op score on the screen.
 */
void view_display_op_scores(int points, int game, int set)
{
	char styled_text[1024];
	const char *op_score_str;

	switch (points) {
	case POINT_LOVE:
		op_score_str = "0";
		break;
	case POINT_15:
		op_score_str = "15";
		break;
	case POINT_30:
		op_score_str = "30";
		break;
	case POINT_40:
		op_score_str = "40";
		break;
	default:
		break;
	}

	dlog_print(DLOG_INFO, LOG_TAG, "op displayed points: %s", op_score_str);
	snprintf(styled_text, sizeof(styled_text), "<font_size=50><align=right>%s</align></font_size>", op_score_str);
	elm_object_text_set(s_info.op_points_label, styled_text);

	dlog_print(DLOG_INFO, LOG_TAG, "op displayed game: %d", game);
	snprintf(styled_text, sizeof(styled_text), "<font_size=25><align=right>%d</align></font_size>", game);
	elm_object_text_set(s_info.op_scores_label[1], styled_text);

}

/**
 * @brief Function will be called when the button is pressed showing pressed effect.
 * @param[in] data Information of the pressed button
 * @param[in] obj Pressed button
 * @param[in] event_info Information of the pressed event
 */
static void _button_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	button_score *btn_score = (button_score *) data;
	dlog_print(DLOG_INFO, LOG_TAG, "button is down [ type: %d, %s ]", btn_score->button_type, btn_score->button_name);
	/* Send signal to the EDJ file to change the state as 'press' */
	elm_object_signal_emit(s_info.layout, "mouse_down", btn_score->button_name);
}

/**
 * @brief Function will be called when the button is up showing default effect.
 * @param[in] data Information of the upped button
 * @param[in] obj Upped button
 * @param[in] event_info Information of the upped event
 */
static void _button_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	button_score *btn_score = (button_score *) data;
	dlog_print(DLOG_INFO, LOG_TAG, "button is up [ type: %d, %s ]", btn_score->button_type, btn_score->button_name);
	/* Send signal to the EDJ file to change the state as 'default' */
	elm_object_signal_emit(s_info.layout, "mouse_up", btn_score->button_name);
}

/**
 * @brief Function will be operated when window is deleted.
 * @param[in] data The data to be passed to the callback function
 * @param[in] obj The evas object handle to be passed to the callback function
 * @param[in] event_info The system event information
 */
static void _win_delete_request_cb(void *user_data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

/**
 * @brief Layout back key event callback function.
 * @param[in] data The data to be passed to the callback function
 * @param[in] obj The evas object handle to be passed to the callback function
 * @param[in] event_info The system event information
 */
static void _layout_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}
