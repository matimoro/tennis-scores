#include <dlog.h>
#include <efl_extension.h>
#include <app_common.h>
#include <main.h>
#include <media_content.h>
#include "data.h"

static struct score my_score = {
	.point_won = POINT_LOVE,
	.game_won = GAME_ZERO,
	.set_won = SET_ZERO,
};

static struct score op_score = {
	.point_won = POINT_LOVE,
	.game_won = GAME_ZERO,
	.set_won = SET_ZERO
};

/*
 * @brief Gets the my score.
 */
struct score *data_get_my_score(void)
{
	return &my_score;
}

/*
 * @brief Gets the opponent score.
 */
struct score *data_get_opponent_score(void)
{
	return &op_score;
}

/*
 * @brief Add my score.
 */
void data_add_my_score(button_score *btn_score)
{
	if (btn_score == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "My score button is NULL");
	}

	my_score.point_won = my_score.point_won + 1;

	if (my_score.point_won == POINT_GAME){
		my_score.point_won = POINT_LOVE;
		my_score.game_won = my_score.game_won + 1;
	}

	if (my_score.game_won == GAME_SET){
		my_score.game_won = GAME_ZERO;
		my_score.set_won = my_score.set_won + 1;
	}

	if (my_score.set_won == SET_MATCH){
		my_score.set_won = SET_ZERO;
		dlog_print(DLOG_INFO, LOG_TAG, "YOU WIN THE MATCH! CONGRATULATIONS!");
	}

	dlog_print(DLOG_INFO, LOG_TAG, "my_score.point_won: %d", my_score.point_won);
	dlog_print(DLOG_INFO, LOG_TAG, "my_score.game_won: %d", my_score.game_won);
	dlog_print(DLOG_INFO, LOG_TAG, "my_score.set_won: %d", my_score.set_won);

}

/*
 * @brief Add opponent score.
 */
void data_add_opponent_score(button_score *btn_score)
{
	if (btn_score == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Opponent button is NULL");
	}

	op_score.point_won = op_score.point_won + 1;

	if (op_score.point_won == POINT_GAME){
		op_score.point_won = POINT_LOVE;
		op_score.game_won = op_score.game_won + 1;
	}

	if (op_score.game_won == GAME_SET){
		op_score.game_won = GAME_ZERO;
		op_score.set_won = op_score.set_won + 1;
	}

	if (op_score.set_won == SET_MATCH){
		op_score.set_won = SET_ZERO;
		dlog_print(DLOG_INFO, LOG_TAG, "YOU WIN THE MATCH! CONGRATULATIONS!");
	}

	dlog_print(DLOG_INFO, LOG_TAG, "op_score.point_won: %d", op_score.point_won);
	dlog_print(DLOG_INFO, LOG_TAG, "op_score.game_won: %d", op_score.game_won);
	dlog_print(DLOG_INFO, LOG_TAG, "op_score.set_won: %d", op_score.set_won);

}

/*
 * @brief Gets path of resource.
 * @param[in] file_in File path of target file
 * @param[out] file_path_out Full file path concatenated with resource path
 * @param[in] file_path_max Max length of full file path
 */
void data_get_resource_path(const char *file_in, char *file_path_out, int file_path_max)
{
	char *res_path = app_get_resource_path();
	if (res_path) {
		snprintf(file_path_out, file_path_max, "%s%s", res_path, file_in);
		free(res_path);
	}
}
