#if !defined(_DATA_H)
#define _DATA_H

typedef enum {
	POINT_LOVE = 0,
	POINT_15 = 1,
	POINT_30 = 2,
	POINT_40 = 3,
	POINT_GAME = 4,
} point;

typedef enum {
	GAME_ZERO = 0,
	GAME_ONE = 1,
	GAME_TWO = 2,
	GAME_THREE = 3,
	GAME_FOUR = 4,
	GAME_FIVE = 5,
	GAME_SET = 6,
} game;

typedef enum {
	SET_ZERO = 0,
	SET_ONE = 1,
	SET_TWO = 2,
	SET_MATCH = 3,
} set;

typedef enum {
	KEY_TYPE_ME = 0,
	KET_TYPE_OPPONENT = 1,
} key_type;

struct score {
	point point_won;
	game game_won;
	set set_won;
};

typedef struct {
	Evas_Object *button;
	key_type button_type;
	const char *button_name;
} button_score;

struct score *data_get_my_score(void);
struct score *data_get_opponent_score(void);
void data_add_my_score(button_score *btn_score);
void data_add_opponent_score(button_score *btn_score);
void data_get_resource_path(const char *file_in, char *file_path_out, int file_path_max);

#endif
