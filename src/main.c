#include <main.h>
#include "view.h"
#include "data.h"

static button_score my_score_button = {
		.button = NULL,
		.button_type = KEY_TYPE_ME,
		.button_name = "my_score_button"
};

static button_score opponent_score_button = {
		.button = NULL,
		.button_type = KET_TYPE_OPPONENT,
		.button_name = "op_score_button"
};

/**
 * @brief Function will be called when the button is clicked determining
 * what kind of button is clicked.
 * @param[in] data Information of the clicked button
 * @param[in] obj Clicked button
 * @param[in] event_info Information of the clicked event
 */
static void button_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	button_score *btn_score = (button_score *) data;
	dlog_print(DLOG_INFO, LOG_TAG, "button is clicked [ type: %d, %s ]", btn_score->button_type, btn_score->button_name);

	switch (btn_score->button_type) {
	case KEY_TYPE_ME:
		data_add_my_score(btn_score);
		break;
	case KET_TYPE_OPPONENT:
		data_add_opponent_score(btn_score);
		break;
	default:
		break;
	}

	struct score *my_score = NULL;
	my_score = data_get_my_score();
	view_display_my_scores(my_score->point_won, my_score->game_won, my_score->set_won);

	struct score *op_score = NULL;
	op_score = data_get_opponent_score();
	view_display_op_scores(op_score->point_won, op_score->game_won, op_score->set_won);
}

/**
 * @brief Hook to take necessary actions before main event loop starts.
 * @param[in] user_data The user data to be passed to the callback function
 * Initialize UI resources and application's data
 * If this function returns true, the main loop of application starts
 * If this function returns false, the application is terminated
 */
static bool app_create(void *data)
{
	char file_path[PATH_MAX] = { 0, };

	/* Get the path of EDJ file */
	data_get_resource_path(EDJ_FILE, file_path, sizeof(file_path));

	/* Initialize the new theme using EDJ file */
	view_init_tennis_scores_theme(file_path);

	/* Create window, conformant */
	view_create();

	/* Create specialized layout for the calculator using EDJ file */
	view_create_tennis_scores_layout(file_path, GRP_MAIN);

	/* Create label display the formula on the screen */
	view_create_my_points_label();
	view_create_op_points_label();
	view_create_my_scores_label();
	view_create_op_scores_label();

	/* Create each button for the app */
	view_create_scores_button(&my_score_button, button_clicked_cb);
	view_create_scores_button(&opponent_score_button, button_clicked_cb);

	return true;
}

/**
 * @brief This callback function is called when another application.
 * @param[in] app_control The handle to the app_control
 * @param[in] user_data The user data to be passed to the callback function
 * sends the launch request to the application
 */
static void app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

/**
 * @brief This callback function is called each time.
 * @param[in] user_data The user data to be passed to the callback function
 * the application is completely obscured by another application
 * and becomes invisible to the user
 */
static void app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

/**
 * @brief This callback function is called each time.
 * @param[in] user_data The user data to be passed to the callback function
 * the application becomes visible to the user
 */
static void app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

/**
 * @brief This callback function is called once after the main loop of the application exits.
 * @param[in] user_data The user data to be passed to the callback function
 */
static void app_terminate(void *data)
{
	/* Release all resources. */
	char file_path[PATH_MAX] = {0, };

	/* Get the EDJ file path */
	data_get_resource_path(EDJ_FILE, file_path, sizeof(file_path));

	/* Finalize the theme using EDJ file */
	view_fini_tennis_scores_theme(file_path);

	/* Destroy the window */
	view_destroy();
}

/**
 * @brief This function will be called when the language is changed.
 * @param[in] event_info The system event information
 * @param[in] user_data The user data to be passed to the callback function
 */
static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);

	if (locale != NULL) {
		elm_language_set(locale);
		free(locale);
	}
	return;
}

/**
 * @brief Main function of the application.
 * @param[in] argc The argument count
 * @param[in] argv The argument vector
 */
int main(int argc, char *argv[])
{
	int ret;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, NULL);

	ret = ui_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);
	}

	return ret;
}
