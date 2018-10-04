#include <string>
#include <Windows.h>

#include <Engine/Keys.h>
#include <Engine/Input.h>
#include <Engine/InputEvents.h>
#include <Engine/Sprite.h>

#include "Game.h"

/**
*   @brief   Default Constructor.
*   @details Consider setting the game's width and height
             and even seeding the random number generator.
*/
BreakoutGame::BreakoutGame()
{
}

/**
*   @brief   Destructor.
*   @details Remove any non-managed memory and callbacks.
*/
BreakoutGame::~BreakoutGame()
{
	this->inputs->unregisterCallback(key_callback_id);
	this->inputs->unregisterCallback(mouse_callback_id);

	gameplay_area.spriteComponent()->freeSprite();
	paddle.spriteComponent()->freeSprite();
	ball.spriteComponent()->freeSprite();
	power_up.spriteComponent()->freeSprite();
	heart.spriteComponent()->freeSprite();

	for (int i = 0; i < MAX_BLOCKS; i++)
	{
			blocks[i].spriteComponent()->freeSprite();
	}
	for (int i = 0; i < MAX_GEMS; i++)
	{
		gems[i].spriteComponent()->freeSprite();
	}
	for (int i = 0; i < MAX_LASERS; i++)
	{
		lasers[i].spriteComponent()->freeSprite();
	}

}

/**
*   @brief   Initialises the game.
*   @details The game window is created and all assets required to
			 run the game are loaded. The keyHandler and clickHandler
			 callback should also be set in the initialise function.
*   @return  True if the game initialised correctly.
*/
bool BreakoutGame::init()
{
	setupResolution();
	if (!initAPI())
	{
		return false;
	}
	renderer->setWindowTitle("Breakout!");

	// input handling functions
	inputs->use_threads = false;

	key_callback_id = inputs->addCallbackFnc(
		ASGE::E_KEY, &BreakoutGame::keyHandler, this);
	
	mouse_callback_id =inputs->addCallbackFnc(
		ASGE::E_MOUSE_CLICK, &BreakoutGame::clickHandler, this);

	clearArrays();
	loadFiles();

	if (!gameplay_area.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\puzzlepack\\png\\background.png"))
	{
		return false;
	}
	if (!ball.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\puzzlepack\\png\\ballBlue.png"))
	{
		return false;
	}
	if (!paddle.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\puzzlepack\\png\\paddleBlue.png"))
	{
		return false;
	}
	if (!heart.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\puzzlepack\\png\\heart.png"))
	{
		return false;
	}

	ASGE::Sprite* background_sprite = gameplay_area.spriteComponent()->getSprite();
	background_sprite->height(game_height * GAMEPLAY_HEIGHT);
	background_sprite->width(background_sprite->height() * 1.25f);
	background_sprite->yPos(game_height * .09f);
	background_sprite->xPos((game_width - background_sprite->width()) * 0.5f);


	ASGE::Sprite* paddle_sprite = paddle.spriteComponent()->getSprite();
	paddle_sprite->height(background_sprite->height() * .03f);
	paddle_sprite->width(background_sprite->height() * .15f);
	paddle_sprite->yPos(background_sprite->yPos() + background_sprite->height()
		- paddle_sprite->height());
	paddle_sprite->xPos(background_sprite->xPos() + (background_sprite->width()
		* 0.5f) - (paddle_sprite->width() * 0.5f));

	ASGE::Sprite* ball_sprite = ball.spriteComponent()->getSprite();
	ball_sprite->height(background_sprite->height() * .03f);
	ball_sprite->width(background_sprite->height() * .03f);
	ball_sprite->yPos(background_sprite->yPos() + background_sprite->height()
		- (paddle_sprite->height() + ball_sprite->height()) );
	ball_sprite->xPos(background_sprite->xPos() + (background_sprite->width() 
		* 0.5f) - (ball_sprite->width() * 0.5f));


	ASGE::Sprite* heart_sprite = heart.spriteComponent()->getSprite();
	heart_sprite->height(game_height * 0.04f);
	heart_sprite->width(game_height * 0.04f);
	heart_sprite->yPos(game_height * 0.05f);
	heart_sprite->xPos(background_sprite->xPos());


	for (int i = 0; i < MAX_BLOCKS; i++)
	{
		if (i == 32 || i == 42 || i == 80 || i == 84 || i == 106 || i == 118)
		{
			if (!blocks[i].addSpriteComponent(renderer.get(),
				".\\Resources\\Textures\\puzzlepack\\png\\element_purple_rectangle.png"))
			{
				return false;
			}

		}
		else if (i % 2 == 0)
		{
			if (!blocks[i].addSpriteComponent(renderer.get(),
				".\\Resources\\Textures\\puzzlepack\\png\\element_red_rectangle.png"))
			{
				return false;
			}

		}
		else
		{
			if (!blocks[i].addSpriteComponent(renderer.get(),
				".\\Resources\\Textures\\puzzlepack\\png\\element_blue_rectangle.png"))
			{
				return false;
			}
		}
		ASGE::Sprite* block = blocks[i].spriteComponent()->getSprite();
		block->height((game_height * GAMEPLAY_HEIGHT) * .035f);
		block->width((((game_height * GAMEPLAY_HEIGHT) * 1.25f)) * .066f);
	}

	for (int i = 0; i < MAX_GEMS; i++)
	{
		if (!gems[i].addSpriteComponent(renderer.get(),
			".\\Resources\\Textures\\puzzlepack\\png\\element_grey_polygon.png"))
		{
			return false;
		}	
		gems[i].setVisible(false);
		ASGE::Sprite* gem = gems[i].spriteComponent()->getSprite();
		gem->height((game_height * GAMEPLAY_HEIGHT) * .035f);
		gem->width((game_height * GAMEPLAY_HEIGHT) * .035f);
	}

	if (!power_up.addSpriteComponent(renderer.get(),
		".\\Resources\\Textures\\puzzlepack\\png\\element_purple_polygon.png"))
	{
		return false;
	}
	power_up.setVisible(false);
	ASGE::Sprite* power_up_sprite = power_up.spriteComponent()->getSprite();
	power_up_sprite->height((game_height * GAMEPLAY_HEIGHT) * .035f);
	power_up_sprite->width((game_height * GAMEPLAY_HEIGHT) * .035f);

	for (int i = 0; i < MAX_LASERS; i++)
	{
		if (!lasers[i].addSpriteComponent(renderer.get(),
			".\\Resources\\Textures\\puzzlepack\\png\\element_red_square.png"))
		{
			return false;
		}
		lasers[i].setVisible(false);
		ASGE::Sprite* laser = lasers[i].spriteComponent()->getSprite();
		laser->height((game_height * GAMEPLAY_HEIGHT) * .035f);
		laser->width((game_height * GAMEPLAY_HEIGHT) * .01f);
	}

	return true;
}

/**
*   @brief   Sets the game window resolution
*   @details This function is designed to create the window size, any 
             aspect ratio scaling factors and safe zones to ensure the 
			 game frames when resolutions are changed in size.
*   @return  void
*/
void BreakoutGame::setupResolution()
{
	// how will you calculate the game's resolution?
	// will it scale correctly in full screen? what AR will you use?
	// how will the game be framed in native 16:9 resolutions?
	// here are some abritrary values for you to adjust as you see fit
	// https://www.gamasutra.com/blogs/KenanBolukbasi/20171002/306822/Scaling_and_MultiResolution_in_2D_Games.php
	game_height = GetSystemMetrics(SM_CYSCREEN);
	game_width = GetSystemMetrics(SM_CXSCREEN);


}

/**
*   @brief   Processes any key inputs
*   @details This function is added as a callback to handle the game's
			 keyboard input. For this game, calls to this function
			 are thread safe, so you may alter the game's state as you
			 see fit.
*   @param   data The event data relating to key input.
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::keyHandler(const ASGE::SharedEventData data)
{
	auto key = static_cast<const ASGE::KeyEvent*>(data.get());
	
	if (key->key == ASGE::KEYS::KEY_ESCAPE)
	{
		signalExit();
	}

	if (key->key == ASGE::KEYS::KEY_SPACE &&
		key->action == ASGE::KEYS::KEY_PRESSED
		&& game_state == 1 && power_up_bool == true
		&& power_up_shots < MAX_LASERS)
	{
		for (int i = 0; i < MAX_LASERS; i++)
		{
			if (lasers[i].getVisible() == false)
			{
				shootLaser(i);
				return;
			}
		}
	}

	if (key->key == ASGE::KEYS::KEY_UP &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		if (game_state == 0)
		{
			menu_option--;
			if (menu_option < 0 && game_state == 0)
			{
				menu_option = 2;
			}
		}
		else if (game_state == 4)
		{
			new_initial += 1;
			if (new_initial > 'Z')
			{
				new_initial = 'A';
			}
			new_initials[initial] = new_initial;
		}
	}
	if (key->key == ASGE::KEYS::KEY_DOWN &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		if (game_state == 0)
		{
			menu_option++;
			if (menu_option > 2)
			{
				menu_option = 0;
			}
		}
		else if (game_state == 4)
		{
			new_initial -= 1;
			if (new_initial < 'A')
			{
				new_initial = 'Z';
			}
			new_initials[initial] = new_initial;
		}
	}

	if (key->key == ASGE::KEYS::KEY_ENTER &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{   //Main menu
		if (game_state == 0)
		{
			if (menu_option == 0)
			{

				game_state = 1;
				new_game = true;
			}
			if (menu_option == 1)
			{
				game_state = 5;
			}
			if (menu_option == 2)
			{
				signalExit();
			}
		}
		else if (game_state == 2 || game_state == 3 || game_state == 5)
		{
			if (updateHighScores() == true)
			{
				game_state = 4;
			}
			else
			{
				game_state = 0;
			}
		}
		else if (game_state == 4)
		{
			high_scores[high_score_idx_to_update].initials = new_initials;
			saveHighScores();

			// reset menu variables
			initial = 0;
			new_initial = 'A';
			new_initials = "AAA";
			high_score_idx_to_update = 0;
			game_state = 0;
		}

	}
	if (key->key == ASGE::KEYS::KEY_A &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		//Main menu
		if (game_state == 1)
		{

			paddle.setVelocity(0.0f, 0.0f);
		}
	}

	if (key->key == ASGE::KEYS::KEY_S &&
		key->action == ASGE::KEYS::KEY_RELEASED)
	{
		//Main menu
		if (game_state == 1)
		{
			paddle.setVelocity(0.0f, 0.0f);
		}
	}

	if (key->key == ASGE::KEYS::KEY_S &&
		(key->action == ASGE::KEYS::KEY_PRESSED ||
			key->action == ASGE::KEYS::KEY_REPEATED))
	{
		//Main menu
		if (game_state == 1)
		{
			paddle.setVelocity(1.0f, 0.0f);
		}
	}

	if (key->key == ASGE::KEYS::KEY_A &&
		(key->action == ASGE::KEYS::KEY_PRESSED ||
		key->action == ASGE::KEYS::KEY_REPEATED))
	{
		//Main menu
		if (game_state == 1)
		{
			paddle.setVelocity(-1.0f, 0.0f);
		}
	}
	if (key->key == ASGE::KEYS::KEY_LEFT &&
		key->action == ASGE::KEYS::KEY_PRESSED && game_state == 4)
	{
		initial -= 1;
		if (initial < 0)
		{
			initial = 2;
		}
		new_initial = new_initials[initial];
	}
	if (key->key == ASGE::KEYS::KEY_RIGHT &&
		key->action == ASGE::KEYS::KEY_PRESSED && game_state == 4)
	{
		initial += 1;
		if (initial > 2)
		{
			initial = 0;
		}
		new_initial = new_initials[initial];
	}
}

/**
*   @brief   Processes any click inputs
*   @details This function is added as a callback to handle the game's
		     mouse button input. For this game, calls to this function
             are thread safe, so you may alter the game's state as you
             see fit.
*   @param   data The event data relating to key input.
*   @see     ClickEvent
*   @return  void
*/
void BreakoutGame::clickHandler(const ASGE::SharedEventData data)
{
	auto click = static_cast<const ASGE::ClickEvent*>(data.get());

	double x_pos, y_pos;
	inputs->getCursorPos(x_pos, y_pos);
}

/**
*   @brief   Updates the scene
*   @details Prepares the renderer subsystem before drawing the
		     current frame. Once the current frame is has finished
		     the buffers are swapped accordingly and the image shown.
*   @return  void
*/
void BreakoutGame::update(const ASGE::GameTime& us)
{
	if (game_state == 1)
	{
		if (new_game)
		{
			newGame();
		}
		auto dt_sec = us.delta_time.count() / 1000.0;
		vector2 paddle_velocity = paddle.getVelocity();

		ASGE::Sprite* background_sprite = gameplay_area.spriteComponent()->getSprite();
		ASGE::Sprite* paddle_sprite = paddle.spriteComponent()->getSprite();
		ASGE::Sprite* ball_sprite = ball.spriteComponent()->getSprite();
		//make sure you use delta time in any movement calculations!
		if ((paddle_sprite->xPos() <= background_sprite->xPos() &&
			paddle_velocity.getX() == -1.f) || ((paddle_sprite->xPos() +
				paddle_sprite->width()) >= (background_sprite->xPos() +
					background_sprite->width()) && paddle_velocity.getX() == 1.f))
		{
			paddle_velocity.setX(0.f);
		}
		blockCollision();
		paddleCollision();

		
		vector2 ball_velocity = ball.getVelocity();
		paddle_sprite->xPos((float)(paddle_sprite->xPos() +
			(paddle_velocity.getX()  * (game_height * 0.45f)) *
			(us.delta_time.count() / 1000.f)));
		ball_sprite->xPos((float)(ball_sprite->xPos() +
			(ball_velocity.getX() * (game_height * 0.5f)) * game_speed *	
			(us.delta_time.count() / 1000.f)));
		ball_sprite->yPos((float)(ball_sprite->yPos() +
			(ball_velocity.getY() * (game_height * 0.5f)) * game_speed *
			(us.delta_time.count() / 1000.f)));

		for (int i = 0; i < MAX_GEMS; i++)
		{
			ASGE::Sprite* gem = gems[i].spriteComponent()->getSprite();
			vector2 gem_velocity = gems[i].getVelocity();
			gem->yPos((float)(gem->yPos() +
				(gem_velocity.getY()  * (game_height * 0.45f)) *	
				(us.delta_time.count() / 1000.f)));
		}

		ASGE::Sprite* power_up_sprite = power_up.spriteComponent()->getSprite();
		vector2 power_up_velocity = power_up.getVelocity();
		power_up_sprite->yPos((float)(power_up_sprite->yPos() +
			(power_up_velocity.getY()  * (game_height * 0.45f)) *
			(us.delta_time.count() / 1000.f)));
		if (power_up_shots == MAX_LASERS)
		{
			power_up_bool = false;
		}
		for (int i = 0; i < MAX_LASERS; i++)
		{
			ASGE::Sprite* laser = lasers[i].spriteComponent()->getSprite();
			vector2 laser_velocity = lasers[i].getVelocity();
			laser->yPos((float)(laser->yPos() +
				(laser_velocity.getY()  * (game_height * 0.45f)) *
				(us.delta_time.count() / 1000.f)));
		}
	}




}

/**
*   @brief   Renders the scene
*   @details Renders all the game objects to the current frame.
	         Once the current frame is has finished the buffers are
			 swapped accordingly and the image shown.
*   @return  void
*/
void BreakoutGame::render(const ASGE::GameTime &)
{
	renderer->setFont(0);

	if (game_state == 0)
	{
		renderMainMenu();
	}
	else if (game_state == 1)
	{
		renderInGame();
	}
	else if (game_state == 2)
	{
		renderGameOverL();
	}

	else if (game_state == 3)
	{
		renderGameOverW();
	}
	else if (game_state == 4)
	{
		renderNewHighScore();
	}
	else if (game_state == 5)
	{
		renderHighScores();
	}

}

/**
*   @brief   Main menu
*   @details This function is used todisplay the main menu
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::renderMainMenu()
{

	// Set Background colour
	renderer->setClearColour(ASGE::COLOURS::MIDNIGHTBLUE);
	// renders the main menu text
	renderer->renderText(
		"WELCOME TO BREAKOUT \n Press Esc to quit at any time.", game_width * 0.2f,
		game_height * 0.15f, game_height * 0.002f, ASGE::COLOURS::DARKORANGE);

	renderer->renderText(menu_option == 0 ? ">PLAY" : "PLAY", game_width * 0.2f,
		game_height * 0.3f, game_height * 0.002f, ASGE::COLOURS::WHITESMOKE);

	renderer->renderText(menu_option == 1 ? ">HIGH SCORES" : "HIGH SCORES", game_width * 0.2f,
		game_height * 0.4f, game_height * 0.002f, ASGE::COLOURS::WHITESMOKE);

	renderer->renderText(menu_option == 2 ? ">QUIT" : "QUIT", game_width * 0.2f,
		game_height * 0.5f, game_height * 0.002f, ASGE::COLOURS::WHITESMOKE);


	

}

/**
*   @brief   In Game Screen
*   @details This function is used todisplay the main menu
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::renderInGame()
{
	// Set Background colour
	renderer->setClearColour(ASGE::COLOURS::MIDNIGHTBLUE);
	renderer->renderSprite(*gameplay_area.spriteComponent()->getSprite());
	renderer->renderSprite(*paddle.spriteComponent()->getSprite());
	renderer->renderSprite(*ball.spriteComponent()->getSprite());
	renderer->renderSprite(*heart.spriteComponent()->getSprite());

	renderer->renderText("Score: ",
		(game_width * 0.60f), (game_height * 0.088f),
		game_height * 0.002f, ASGE::COLOURS::DARKORANGE);
	std::string score_string = std::to_string(score);
	renderer->renderText(score_string.c_str(),
		(game_width * 0.73f), (game_height * 0.088f),
		game_height * 0.002f, ASGE::COLOURS::DARKORANGE);

	std::string life_string = std::to_string(lives -1);
	rect heart_sprite = heart.spriteComponent()->getBoundingBox();
	renderer->renderText(life_string.c_str(),
		(heart_sprite.x + (heart_sprite.length * 1.02f)),
		(heart_sprite.y + (heart_sprite.height * 0.95f)),
		game_height * 0.0025f, ASGE::COLOURS::DARKORANGE);


	if (power_up.getVisible() == true)
	{
		renderer->renderSprite(*power_up.spriteComponent()->getSprite());
	}
	for (int i = 0; i < MAX_GEMS; i++)
	{
		if (gems[i].getVisible() == true)
		{
			renderer->renderSprite(*gems[i].spriteComponent()->getSprite());
		}

	}
	for (int i = 0; i < MAX_LASERS; i++)
	{
		if (lasers[i].getVisible() == true)
		{
			renderer->renderSprite(*lasers[i].spriteComponent()->getSprite());
		}

	}

	for (int i = 0; i < MAX_BLOCKS; i++)
	{
		if (blocks[i].getVisible() == true)
		{
			renderer->renderSprite(*blocks[i].spriteComponent()->getSprite());
		}
		
	}
}

/**
*   @brief   Game Over loss
*   @details This function is used to display the game over screen
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::renderGameOverL()
{
	// Set Background colour
	renderer->setClearColour(ASGE::COLOURS::BLACK);
	// renders the main menu text
	renderer->renderText(
		"GAME OVER out of lives \n Press Enter to return to main menu.",
		game_width * 0.25f,	game_height * 0.2f, game_height * 0.002f, ASGE::COLOURS::DARKORANGE);
	renderer->renderText("Final Score: ",	(game_width * 0.3f), (game_height * 0.5f),
			game_height * 0.004f, ASGE::COLOURS::WHITESMOKE);
	std::string score_string = std::to_string(score);
	renderer->renderText(score_string.c_str(),
		(game_width * 0.7f), (game_height * 0.50f),
		game_height * 0.004f, ASGE::COLOURS::WHITESMOKE);

}

/**
*   @brief   Game Over Win
*   @details This function is used todisplay the the game over screen
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::renderGameOverW()
{
	// Set Background colour
	renderer->setClearColour(ASGE::COLOURS::BLACK);
	// renders the main menu text
	renderer->renderText(
		"CONGRATULATIONS you cleared the game \n Press Enter to return to main menu.", game_width * 0.25f,
		game_height * 0.2f, game_height * 0.002f, ASGE::COLOURS::DARKORANGE);
	renderer->renderText("Final Score: ", (game_width * 0.3f), (game_height * 0.5f),
		game_height * 0.004f, ASGE::COLOURS::WHITESMOKE);
	std::string score_string = std::to_string(score);
	renderer->renderText(score_string.c_str(),
		(game_width * 0.7f), (game_height * 0.50f),
		game_height * 0.004f, ASGE::COLOURS::WHITESMOKE);
}

/**
*   @brief   Update high scores
*   @details This function is used to check and update if single player high
score is higher than the current top ten
*   @see     KeyEvent
*   @return  bool
*/
bool BreakoutGame::updateHighScores()
{
	bool update_score = false;
	high_score_idx_to_update = 0;
	score = (score + (lives * 500));
	for (int i = 9; i > -1; i--)
	{
		if (score > high_scores[i].score)
		{
			high_score_idx_to_update = i;
			update_score = true;
		}
	}
	if (update_score)
	{
		for (int i = 9; i > high_score_idx_to_update; i--)
		{
			high_scores[i].score = high_scores[i - 1].score;
			high_scores[i].initials = high_scores[i - 1].initials;
		}
		high_scores[high_score_idx_to_update].score = score;

	}
	score = 0;
	return update_score;
}

/**
*   @brief   High scores
*   @details This function is used to display the high scores
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::renderHighScores()
{
	// re renders the main menu
	renderMainMenu();


	renderer->renderText("HIGH SCORES", game_width * 0.68f, game_height * 0.15f, game_height * 0.002f,
		ASGE::COLOURS::DARKORANGE);
	// renders the high scores
	int j = 0;
	for (int i = game_height * 0.25f; i < game_height * 0.75f; i = i + game_height * 0.05f)
	{
		renderer->renderText(high_scores[j].initials.c_str(), game_width * 0.7f, i, game_height * 0.002f,
			ASGE::COLOURS::GHOSTWHITE);
		// creates a string with the score appended
		std::string score_str_1 = std::to_string(high_scores[j].score);
		renderer->renderText(score_str_1.c_str(), game_width * 0.75f, i, game_height * 0.002f,
			ASGE::COLOURS::GHOSTWHITE);
		j++;
	}
	renderer->renderText("Press Enter to return to Main Menu", game_width * 0.5f, game_height * 0.8f,
		game_height * 0.002f, ASGE::COLOURS::GHOSTWHITE);
}

/**
*   @brief   New High score
*   @details This function is used to display the high score and update the
players initials
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::renderNewHighScore()
{

	renderer->renderText("CONGRATULATIONS YOU SCORED A NEW HIGH SCORE",
		game_width * 0.1f, game_height * 0.15f, game_height * 0.003f, ASGE::COLOURS::DARKORANGE);
	int j = 0;
	for (int i = game_height * 0.25f; i < game_height * 0.75f; i = i + game_height * 0.05f)
	{
		renderer->renderText(high_score_idx_to_update == j ?
			new_initials.c_str() : high_scores[j].initials.c_str(),
			game_width * 0.45f, i, game_height * 0.002f, high_score_idx_to_update == j ?
			ASGE::COLOURS::GHOSTWHITE : ASGE::COLOURS::DARKORANGE);
		std::string score_str_1 = std::to_string(high_scores[j].score);
		renderer->renderText(score_str_1.c_str(), game_width * 0.5f, i, game_height * 0.002f,
			high_score_idx_to_update == j ?
			ASGE::COLOURS::GHOSTWHITE : ASGE::COLOURS::DARKORANGE);
		j++;
	}



	renderer->renderText(
		"Use arrow keys to change initials and press Enter when finished", game_width * 0.1f, game_height * 0.8f,
		game_height * 0.002f, ASGE::COLOURS::GHOSTWHITE);
}

/**
*   @brief   New Game
*   @details This function is used to reset a new game
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::newGame()
{

	float new_yPos = game_height * .09f + ((game_height * 0.8f) * .06f);
	float new_xPos = ((game_width - ((game_height * GAMEPLAY_HEIGHT) * 1.25f)) * 0.506f);

	// re-initialise blocks
	for (int i = 0; i < MAX_BLOCKS; i++)
	{
		blocks[i].setVisible(true);
		ASGE::Sprite* block = blocks[i].spriteComponent()->getSprite();
		block->yPos(new_yPos);
		block->xPos(new_xPos);
		new_xPos = block->xPos() + block->width();
		if (i % 15 == 14)
		{
			new_yPos += block->height();
			new_xPos = ((game_width - ((game_height * GAMEPLAY_HEIGHT) * 1.25f)) * 0.506f);
		}
	}
	// re-initialise gems
	for (int i = 0; i <MAX_GEMS; i++)
	{
		resetGem(i);
	}
	// re-initialise lasers
	for (int i = 0; i <MAX_LASERS; i++)
	{
		resetLaser(i);
	}
	// re-initialise game variables
	score = 0;
	game_speed = 1.f;
	no_gems_visible = 0;
	no_hit = 0;
	lives = 4;
	paddle.setVelocity(0.f, 0.f);
	serveBall();
	power_up_bool = false;
	power_up_shots = 0;
	new_game = false;
}

/**
*   @brief   serve ball
*   @details This function is used to reset the ball position and serve the ball
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::serveBall()
{
	// set ball position to center of paddle and velocity to straight up
	rect background_sprite = gameplay_area.spriteComponent()->getBoundingBox();
	rect paddle_sprite = paddle.spriteComponent()->getBoundingBox();
	ASGE::Sprite* ball_sprite = ball.spriteComponent()->getSprite();
	ball_sprite->yPos(background_sprite.y + background_sprite.height
		- (paddle_sprite.height + ball_sprite->height()));
	ball_sprite->xPos((paddle_sprite.x  + paddle_sprite.length
		* 0.5f) - (ball_sprite->width() * 0.5f));
	ball.setVelocity(0.0f, -1.0f);
}

/**
*   @brief   Collision detection Blocks
*   @details This function is used to detect collisions with the blocks
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::blockCollision()
{
	rect background = gameplay_area.spriteComponent()->getBoundingBox();
	rect ball_sprite = ball.spriteComponent()->getBoundingBox();
	vector2 ball_velocity = ball.getVelocity();

	// height and width of gameplay area deflections for ball
	if ((ball_sprite.x <= background.x &&
		ball_velocity.getX() < 0.f) || ((ball_sprite.x +
			ball_sprite.length) >= (background.x +
				background.length) && ball_velocity.getX() > 0.f))
	{

		ball_velocity.setX(0 - ball_velocity.getX());
	}
	if (ball_sprite.y <= background.y && ball_velocity.getY() < 0.f)
	{

		ball_velocity.setY(0 - ball_velocity.getY());
	}

	// edge detection for lasers
	for (int i = 0; i < MAX_LASERS; i++)
	{
		rect laser_rect = lasers[i].spriteComponent()->getBoundingBox();
		if (laser_rect.y <= background.y)
		{
			resetLaser(i);
		}

	}

	// block collision detection with ball and lasers
	for (int i = 0; i < MAX_BLOCKS; i++)
	{
		rect block = blocks[i].spriteComponent()->getBoundingBox(); 
		if (blocks[i].getVisible())
		{
			for (int j = 0; j < MAX_LASERS; j++)
			{

				// laser collision check
				rect laser_rect = lasers[j].spriteComponent()->getBoundingBox();
				if ((laser_rect.y < block.y + block.height) && (laser_rect.x > block.x - 
					(laser_rect.length * .5f) && laser_rect.x +
						laser_rect.length  < block.x + block.length +
						(laser_rect.length * .5f)) && lasers[j].getVisible())
				{
					resetLaser(j);
					releaseGem(block);
					releasePowerUp(i, block);
					blocks[i].setVisible(false);
					no_hit++;
					score += 5;
				}
			}

			// ball collision check
			if ((ball_sprite.y < block.y + block.height &&
				ball_sprite.y + ball_sprite.height > block.y &&
				(ball_sprite.x + (ball_sprite.length * 0.9f) > block.x &&
					ball_sprite.x   < block.x + block.length * .9f) &&
				ball_velocity.getY() < 0.f) || (ball_sprite.y  < block.y  &&
					ball_sprite.y + ball_sprite.height > block.y &&
					(ball_sprite.x + (ball_sprite.length * 0.9f) > block.x &&
						ball_sprite.x   < block.x + block.length * .9f) &&
					ball_velocity.getY() > 0.f))
			{
				releaseGem(block);
				ball_velocity.setY(0 - ball_velocity.getY());
				releasePowerUp(i, block);
				blocks[i].setVisible(false);
				no_hit++;
				score += 5;
			}
			else if ((ball_sprite.y < block.y + block.height &&
				ball_sprite.y + (ball_sprite.height)> block.y &&
				(ball_sprite.x + ball_sprite.length > block.x &&
					ball_sprite.x  < block.x) && ball_velocity.getX() > 0.f) ||
					(ball_sprite.y  < block.y + block.height &&
						ball_sprite.y + (ball_sprite.height) > block.y &&
						(ball_sprite.x  < block.x + block.length &&
							ball_sprite.x + ball_sprite.length > block.x + block.length)
						&& ball_velocity.getX() < 0.f))
			{
				releaseGem(block);
				ball_velocity.setX(0 - ball_velocity.getX());
				releasePowerUp(i, block);
				blocks[i].setVisible(false);
				no_hit++;
				score += 5;
			}
		}
		// game over check (win)
		if (no_hit == MAX_BLOCKS)
		{
			game_state = 3;
		}

	}
	// game over check (loss)
	if (lives == 0)
	{
		game_state = 2;
	}
	// update ball velocity
	ball.setVelocity(ball_velocity.getX(), ball_velocity.getY());

}

/**
*   @brief   Collision detection paddle
*   @details This function is used to serve the ball
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::paddleCollision()
{
	rect paddle_sprite = paddle.spriteComponent()->getBoundingBox();
	rect ball_sprite = ball.spriteComponent()->getBoundingBox();
	vector2 ball_velocity = ball.getVelocity();

	// ball/paddle collision detection
	if (ball_sprite.y + ball_sprite.height > paddle_sprite.y)
	{
		if ((ball_sprite.x + ball_sprite.length) < paddle_sprite.x ||
			(ball_sprite.x  > paddle_sprite.x + paddle_sprite.length))
		{
			if (ball_sprite.y > paddle_sprite.y)
			{
				lives--;
				serveBall();
				return;
			}
		}
		else if (ball_sprite.x + (ball_sprite.length) >= paddle_sprite.x +
			paddle_sprite.length)
		{
			ball_velocity.setX(0.707f);
			ball_velocity.setY(-0.707f);
		}
		else if (ball_sprite.x + (ball_sprite.length * 0.5f) >= paddle_sprite.x +
			(paddle_sprite.length * 0.71f))
		{
			ball_velocity.setX(0.5f);
			ball_velocity.setY(-0.866f);
		}
		else if (ball_sprite.x + (ball_sprite.length * 0.5f) >= paddle_sprite.x +
			(paddle_sprite.length * 0.57f))
		{
			ball_velocity.setX(0.259f);
			ball_velocity.setY(-0.966f);
		}
		else if (ball_sprite.x + (ball_sprite.length * 0.5f) >= paddle_sprite.x +
			(paddle_sprite.length * 0.43f))
		{
			ball_velocity.setX(0.f);
			ball_velocity.setY(-1.f);
		}
		else if (ball_sprite.x + (ball_sprite.length * 0.5f) >= paddle_sprite.x +
			(paddle_sprite.length * 0.29f))
		{
			ball_velocity.setX(-0.259f);
			ball_velocity.setY(-0.966f);
		}
		else if (ball_sprite.x + (ball_sprite.length * 0.5f) >= paddle_sprite.x +
			(paddle_sprite.length  * 0.14))
		{
			ball_velocity.setX(-0.500f);
			ball_velocity.setY(-0.866f);
		}
		else if (ball_sprite.x + (ball_sprite.length) >= paddle_sprite.x)
		{
			ball_velocity.setX(-0.707f);
			ball_velocity.setY(-0.707f);
		}
	}
	//upadate ball velocity from collision
	ball.setVelocity(ball_velocity.getX(), ball_velocity.getY());

	// check for gem collision with paddle or bottom of gameplay area
	for (int i = 0; i < MAX_GEMS; i++)
	{
		if (gems[i].getVisible() == true);
		{
			rect gem = gems[i].spriteComponent()->getBoundingBox();
			if (gem.y + gem.height > paddle_sprite.y)
			{
				if (gem.x > paddle_sprite.x && gem.x + gem.length
					< paddle_sprite.x + paddle_sprite.length)
				{
					score += 100;
					resetGem(i);

				}
				else if (gem.y > paddle_sprite.y)
				{
					resetGem(i);
				}
			}

		}
	}

	// check for power up collision with paddle or bottom of gameplay area
	if (power_up.getVisible() == true);
	{
		rect power_up_rect = power_up.spriteComponent()->getBoundingBox();
		if (power_up_rect.y + power_up_rect.height > paddle_sprite.y)
		{
			if (power_up_rect.x > paddle_sprite.x && power_up_rect.x 
				+ power_up_rect.length < paddle_sprite.x + paddle_sprite.length)
			{
				power_up_bool = true;
				power_up_shots = 0;
				resetPowerUp();
			}
			else if (power_up_rect.y > paddle_sprite.y)
			{
				resetPowerUp();
			}
		}

	}

}

/**
*   @brief   Release Gem
*   @details This function is used to release a gem from the 
provided block co-ordinates
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::releaseGem(rect block)
{
	// for every fifth block hit release a gem from the corresponding block
	if (no_hit % 5 == 4)
	{
		ASGE::Sprite* gem = gems[no_gems_visible].spriteComponent()->getSprite();
		gem->yPos(block.y);
		gem->xPos(block.x + ((block.length * 0.5f) - (gem->width() * 0.5f)));
		gems[no_gems_visible].setVelocity(0.f, 0.5f);
		gems[no_gems_visible].setVisible(true);
		no_gems_visible++;
	}
	if (no_hit % 25 == 24)
	{
		game_speed += 0.1f;
	}
}

/**
*   @brief   Release Power Up
*   @details This function is used to release a power up from the
provided block co-ordinates
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::releasePowerUp(int index, rect block)
{
	// relesae poer up from specific block locations
	if (index == 32 || index == 42 || index == 80 || 
		index == 84 || index == 106 || index == 118)
	{
		ASGE::Sprite* power_up_sprite = 
			power_up.spriteComponent()->getSprite();
		power_up_sprite->yPos(block.y);
		power_up_sprite->xPos(block.x + ((block.length * 0.5f)
			- (power_up_sprite->width() * 0.5f)));
		power_up.setVelocity(0.f, 0.45f);
		power_up.setVisible(true);
	}
	
}

/**
*   @brief   Reset Gem
*   @details This function is used to reset a gem's 
position and velocity
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::resetGem(int index)
{
	// reset sprite position, velocity and visibility
	ASGE::Sprite* gem_sprite = gems[index].spriteComponent()->getSprite();
	gem_sprite->yPos(0.f);
	gem_sprite->xPos(0.f);
	gems[index].setVisible(false);
	gems[index].setVelocity(0.f, 0.f);
	no_gems_visible--;
}

/**
*   @brief   Reset Power Up
*   @details This function is used to reset the power up's
position and velocity
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::resetPowerUp()
{
	// reset sprite position, velocity and visibility
	ASGE::Sprite* power_up_sprite = power_up.spriteComponent()->getSprite();
	power_up_sprite->yPos(0.f);
	power_up_sprite->xPos(0.f);
	power_up.setVisible(false);
	power_up.setVelocity(0.f, 0.f);
}

/**
*   @brief   Shoot laser
*   @details This function is used to shoot a laser
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::shootLaser(int index)
{
	// set sprite position for corresponding index to the center of the paddle,
	// set to visible and set velocity to straight up
	no_lasers_visible++;
	rect background_rect = gameplay_area.spriteComponent()->getBoundingBox();
	rect paddle_rect = paddle.spriteComponent()->getBoundingBox();
	ASGE::Sprite* laser_sprite = lasers[index].spriteComponent()->getSprite();
	laser_sprite->yPos(background_rect.y + background_rect.height
		- paddle_rect.height);
	laser_sprite->xPos((paddle_rect.x + paddle_rect.length * 0.5f) - (laser_sprite->width() * 0.5f));
	lasers[index].setVelocity(0.0f, -1.0f);
	lasers[index].setVisible(true);
	power_up_shots++;
}

/**
*   @brief   Reset laser
*   @details This function is used to reset a laser's
position and velocity
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::resetLaser(int index)
{
	// reset sprite position, velocity and visibility
	ASGE::Sprite* laser_sprite = lasers[index].spriteComponent()->getSprite();
	laser_sprite->yPos(0.f);
	laser_sprite->xPos(0.f);
	lasers[index].setVisible(false);
	lasers[index].setVelocity(0.f, 0.f);
	no_lasers_visible--;
}

/**
*   @brief   Load files
*   @details This function is load the high scores and characters files
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::loadFiles()
{
	// load high scores
	std::ifstream inFile_one;
	inFile_one.open("High_scores.txt");
	if (!inFile_one.fail())
	{
		for (int i = 0; i < NUM_HIGH_SCORES; i++)
		{
			std::string score;
			getline(inFile_one, high_scores[i].initials);
			getline(inFile_one, score);
			high_scores[i].score = atoi(score.c_str());
		}
		inFile_one.close();
	}
}

/**
*   @brief   Save files
*   @details This function is load the high scores and characters files
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::saveHighScores()
{
	// save high scores array to file
	std::ofstream outFile;
	outFile.open("High_scores.txt");
	if (!outFile.fail())
	{
		for (int i = 0; i < NUM_HIGH_SCORES; i++)
		{
			outFile << high_scores[i].initials << std::endl;
			outFile << high_scores[i].score << std::endl;
		}
		outFile.close();
	}

}

/**
*   @brief   clear arrays
*   @details This function is used to initialise arrays.
*   @see     KeyEvent
*   @return  void
*/
void BreakoutGame::clearArrays()
{
	for (int i = 0; i < NUM_HIGH_SCORES; i++)
	{
		high_scores[i].initials = "AAA";
		high_scores[i].score = 0;
	}
}