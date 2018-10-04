#pragma once
#include <string>
#include <Engine/OGLGame.h>
#include <fstream>
#include <iostream>

#include "Constants.h"
#include "GameObject.h"
#include "Rect.h"



struct Score
{
	long score = 0;
	std::string initials;
};

/**
*  An OpenGL Game based on ASGE.
*/
class BreakoutGame :
	public ASGE::OGLGame
{
public:
	BreakoutGame();
	~BreakoutGame();
	virtual bool init() override;

private:
	void keyHandler(const ASGE::SharedEventData data);
	void clickHandler(const ASGE::SharedEventData data);
	void setupResolution();
	void renderMainMenu();
	void renderInGame();
	void renderGameOverL();
	void renderGameOverW();
	void serveBall();
	void newGame();
	void blockCollision();
	void paddleCollision();
	void releaseGem(rect Block);
	void releasePowerUp(int index, rect Block);
	void resetGem(int index);
	void resetPowerUp();
	void shootLaser(int index);
	void resetLaser(int index);

	bool updateHighScores();

	void renderHighScores();

	void renderNewHighScore();

	void loadFiles();

	void saveHighScores();

	void clearArrays();

	virtual void update(const ASGE::GameTime &) override;
	virtual void render(const ASGE::GameTime &) override;

	int  key_callback_id = -1;	        /**< Key Input Callback ID. */
	int  mouse_callback_id = -1;        /**< Mouse Input Callback ID. */

	//Add your GameObjects
	GameObject blocks[MAX_BLOCKS];
	GameObject gems[MAX_GEMS];
	GameObject gameplay_area;
	GameObject paddle;
	GameObject ball;
	GameObject heart; 
	GameObject power_up;
	GameObject lasers[MAX_LASERS];

	// menu variables
	int menu_option = 0;
	int initial = 0;

	// game screen to display
	int game_state = 0;

	// in game variables
	bool new_game = true;
	int lives = 0;
	int score = 0;
	int no_hit = 0;
	int no_gems_visible = 0;
	int no_lasers_visible = 0;
	int power_up_shots = 0;
	bool power_up_bool = false;
	float game_speed = 1.f;

	// high score variables
	Score high_scores[NUM_HIGH_SCORES];
	char new_initial = 'A';
	std::string new_initials = "AAA";
	int high_score_idx_to_update = 0;
};