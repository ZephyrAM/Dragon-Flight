#include "gameMain.h"
#include "player.h"
#include "enemy.h"

LPD3DXSPRITE sprite_handler;

LPDIRECT3DSURFACE9 titleBack;
LPDIRECT3DSURFACE9 firstBack;
LPDIRECT3DSURFACE9 endBack;

LPDIRECT3DTEXTURE9 player_image;
playerType player;

LPDIRECT3DTEXTURE9 lives_image;
spriteType livesFont;

LPDIRECT3DTEXTURE9 life_image;
spriteType life[3];

LPDIRECT3DTEXTURE9 enemy_image1;
enemyType enemy[4];

LPDIRECT3DTEXTURE9 score_image;
spriteType scoreFont;

LPDIRECT3DTEXTURE9 number_image;
spriteType scoreValue[6];

LPDIRECT3DTEXTURE9 firePwr_image;
spriteType firePwr[3];

LPDIRECT3DTEXTURE9 enemyPwr_image;
spriteType enemyPwr[4];

HRESULT result;
long start = GetTickCount();
long fired = GetTickCount();
long firing;

CSound *music_back;
CSound *crash;
CSound *hit;
CSound *fireS;

int n, s;
int e = 0;
bool collide = false;
int stage = 0;

int Game_Init(HWND hwnd)
{
	srand(time(NULL));

	if(!Init_Mouse(hwnd))
	{
		MessageBox(hwnd, "Error initializing the mouse", "Error", MB_OK);
		return 0;
	}

	if(!Init_Keyboard(hwnd))
	{
		MessageBox(hwnd, "Error initializing the keyboard", "Error", MB_OK);
		return 0;
	}

	result = D3DXCreateSprite(d3ddev, &sprite_handler);
	if(result != D3D_OK)
		return 0;

	titleBack = LoadSurface("titleBack.png", NULL);
	if(titleBack == NULL)
		return 0;

	firstBack = LoadSurface("firstBack.png", NULL);
	if(firstBack == NULL)
		return 0;

	endBack = LoadSurface("endBack.png", NULL);
	if(endBack == NULL)
		return 0;

	player_image = LoadTexture("playerImage.png", D3DCOLOR_XRGB(255,200,255));
	if(player_image == NULL)
		return 0;

	player.setX(100);
	player.setY(150);
	player.setWidHeight(40, 32);
	player.setCframe(0);
	player.setLframe(2);
	player.setanimCount(0);
	player.setanimDelay(3);
	player.setMoveX(8);
	player.setMoveY(8);

	lives_image = LoadTexture("livesFont.bmp", D3DCOLOR_XRGB(0,0,0));
	if(lives_image == NULL)
		return 0;

	livesFont.setWidHeight(100,300);
	livesFont.setX(10);
	livesFont.setY(15);

	life_image = LoadTexture("livesImage.png", D3DCOLOR_XRGB(255,200,255));
	if(life_image == NULL)
		return 0;

	for(n = 0; n < 3; n++)
	{
		life[n].setWidHeight(45, 41);
		life[n].setX(120 + (life[n].getWidth() * n));
		life[n].setY(10);
	}

	enemy_image1 = LoadTexture("enemyImage1.png", D3DCOLOR_XRGB(255,200,255));
	if(enemy_image1 == NULL)
		return 0;

	for(n = 0; n < 4; n++)
	{
		enemy[n].setWidHeight(35,44);
		enemy[n].setCframe(0);
		enemy[n].setLframe(2);
		enemy[n].setanimCount(0);
		enemy[n].setanimDelay(3);
	}
	enemy[0].setMoveX(8);
	enemy[1].setMoveX(4);
	enemy[2].setMoveX(6);
	enemy[3].setMoveX(10);

	score_image = LoadTexture("scoreFont.bmp", D3DCOLOR_XRGB(0,0,0));
	if(score_image == NULL)
		return 0;

	scoreFont.setWidHeight(100,30);
	scoreFont.setX(380);
	scoreFont.setY(15);

	number_image = LoadTexture("scoreValue.bmp", D3DCOLOR_XRGB(0,0,0));
	if(number_image == NULL)
		return 0;

	for(n = 0; n < 6; n++)
	{
		scoreValue[n].setWidHeight(20,30);
		scoreValue[n].setX(500 + (scoreValue[n].getWidth() * n));
		scoreValue[n].setY(15);
		scoreValue[n].setCframe(0);
		scoreValue[n].setLframe(9);
		scoreValue[n].setanimCount(0);
		scoreValue[n].setanimDelay(10);
	}

	firePwr_image = LoadTexture("firePwrImage.png", D3DCOLOR_XRGB(255,185,255));
	if(firePwr_image == NULL)
		return 0;

	for(n = 0; n < 3; n++)
	{
		firePwr[n].setWidHeight(60,20);
		firePwr[n].setX(-100);
		firePwr[n].setY(-100);
		firePwr[n].setMoveX(18);
	}

	enemyPwr_image = LoadTexture("enemyFireImage.png", D3DCOLOR_XRGB(133,176,182));
	if(enemyPwr_image == NULL)
		return 0;

	for(n = 0; n < 4; n++)
	{
		enemyPwr[n].setWidHeight(46,48);
		enemyPwr[n].setX(-50);
		enemyPwr[n].setY(-50);
		enemyPwr[n].setCframe(0);
		enemyPwr[n].setLframe(2);
		enemyPwr[n].setanimCount(0);
		enemyPwr[n].setanimDelay(3);
		enemyPwr[n].setMoveX(enemy[n].getMoveX() + 5);
	}

	crash = LoadSound("crashSound.wav");
	if(crash == NULL)
		return 0;

	hit = LoadSound("hitSound.wav");
	if(hit == NULL)
		return 0;

	fireS = LoadSound("fireSound.wav");
	if(fireS == NULL)
		return 0;

	music_back = LoadSound("backMusic.wav");
	if(music_back == NULL)
		return 0;

	return 1;
}

int Collision(spriteType sprite1, spriteType sprite2)
{
	RECT rect1;
	rect1.left = sprite1.getX() + 1;
	rect1.top = sprite1.getY() + 1;
	rect1.right = sprite1.getX() + sprite1.getWidth() - 1;
	rect1.bottom = sprite1.getY() + sprite1.getHeight() - 1;

	RECT rect2;
	rect2.left = sprite2.getX() + 1;
	rect2.top = sprite2.getY() + 1;
	rect2.right = sprite2.getX() + sprite2.getWidth() - 1;
	rect2.bottom = sprite2.getY() + sprite2.getHeight() - 1;

	RECT dest;
	return IntersectRect(&dest, &rect1, &rect2);
}

void Restart()
{
	player.setX(100);
	player.setY(150);
	player.setLife(3);
	player.setScore(0);

	for(n = 0; n < 3; n++)
	{
		player.endFire(n);
		player.letShoot(n);

		firePwr[n].setX(-100);
		firePwr[n].setY(-100);
	}

	for(n = 0; n < 4; n++)
	{
		enemy[n].setY((rand() % 376) + 60);
		enemy[n].setX(640);

		enemy[n].setFire(false);
		enemy[n].setShoot(true);

		enemyPwr[n].setX(-50);
		enemyPwr[n].setY(-50);
	}

	for(n = 0; n < 6; n++)
	{
		scoreValue[n].setCframe(0);
	}
}

void Game_Run(HWND hwnd)
{
	if(d3ddev == NULL)
		return;

	LoopSound(music_back);

	D3DXVECTOR3 position(0,0,0);

	Poll_Mouse();
	Poll_Keyboard();

	if(GetTickCount() - start >= 30 && stage != 0 && stage != 4)
	{
		start = GetTickCount();

		if(Key_Down(DIK_RIGHT))
			player.setX(player.getX() + player.getMoveX());
		if(Key_Down(DIK_LEFT))
			player.setX(player.getX() - player.getMoveX());
		if(Key_Down(DIK_DOWN))
			player.setY(player.getY() + player.getMoveY());
		if(Key_Down(DIK_UP))
			player.setY(player.getY() - player.getMoveY());

		if(Key_Down(DIK_SPACE))
		{
			firing = GetTickCount();
			if(player.testShoot(0) == true && firing >= fired + 200)
			{
				player.startFire(0);
				firePwr[0].setX(player.getX());
				firePwr[0].setY(player.getY() + 6);
				player.stopShoot(0);
				fired = GetTickCount();
			}
			if(player.testShoot(0) == false && player.testShoot(1) == true &&
				firing >= fired + 200)
			{
				player.startFire(1);
				firePwr[1].setX(player.getX());
				firePwr[1].setY(player.getY() + 6);
				player.stopShoot(1);
				fired = GetTickCount();
			}
			if(player.testShoot(0) == false && player.testShoot(1) == false &&
				player.testShoot(2) == true && firing >= fired + 200)
			{
				player.startFire(2);
				firePwr[2].setX(player.getX());
				firePwr[2].setY(player.getY() + 6);
				player.stopShoot(2);
				fired = GetTickCount();
			}
		}

		if(player.getX() > SCREEN_WIDTH - player.getWidth())
			player.setX(SCREEN_WIDTH - player.getWidth());
		else if(player.getX() < 0)
			player.setX(0);
		else if(player.getY() > SCREEN_HEIGHT - player.getHeight())
			player.setY(SCREEN_HEIGHT - player.getHeight());
		else if(player.getY() < 60)
			player.setY(60);

		player.setanimCount((player.getanimCount() + 1));
		if(player.getanimCount() > player.getanimDelay())
		{
			player.setanimCount(0);

			player.setCframe((player.getCframe() + 1));
			if(player.getCframe() > player.getLframe())
				player.setCframe(0);
		}

		for(n = 0; n < 4; n++)
		{
			if(enemy[n].getX() < (0 - enemy[n].getWidth()))
			{
				enemy[n].setLife(true);
				enemy[n].setY((rand() % 376) + 60);
				enemy[n].setX(640);
			}
				
			enemy[n].setX(enemy[n].getX() - enemy[n].getMoveX());

			enemy[n].setanimCount((enemy[n].getanimCount() + 1));
			if(enemy[n].getanimCount() > enemy[n].getanimDelay())
			{
				enemy[n].setanimCount(0);

				enemy[n].setCframe((enemy[n].getCframe() + 1));
				if(enemy[n].getCframe() > enemy[n].getLframe())
					enemy[n].setCframe(0);
			}
		}
			
		for(n = 0; n < 4; n++)
		{
			if(enemy[n].getY() >= (player.getY() - 5) && 
				enemy[n].getY() <= (player.getY() + 5) && enemy[n].testShoot() == true)
			{
				enemy[n].setFire(true);
				enemyPwr[n].setX(enemy[n].getX());
				enemyPwr[n].setY(enemy[n].getY() + 6);
				enemy[n].setShoot(false);
			}

			if(enemy[n].testFire() == true)
			{
				enemyPwr[n].setX(enemyPwr[n].getX() - enemyPwr[n].getMoveX());

				enemyPwr[n].setanimCount((enemyPwr[n].getanimCount() + 1));
				if(enemyPwr[n].getanimCount() > enemyPwr[n].getanimDelay())
				{
					enemyPwr[n].setanimCount(0);

					enemyPwr[n].setCframe((enemyPwr[n].getCframe() + 1));
					if(enemyPwr[n].getCframe() > enemyPwr[n].getLframe())
						enemyPwr[n].setCframe(0);
				}

				if(enemyPwr[n].getX() < (0 - enemyPwr[n].getWidth()))
				{
					enemy[n].setFire(false);
					enemyPwr[n].setX(-50);
					enemyPwr[n].setY(-50);
					enemy[n].setShoot(true);
				}
			}
		}

		for(n = 0; n < 3; n++)
		{
			if(player.testFire(n) == true)
			{
				firePwr[n].setX(firePwr[n].getX() + firePwr[n].getMoveX());

				if(firePwr[n].getX() >= SCREEN_WIDTH - firePwr[n].getWidth())
				{
					player.endFire(n);
					firePwr[n].setX(-100);
					firePwr[n].setY(-100);
					player.letShoot(n);
				}
			}
		}

			
		scoreValue[5].setanimCount((scoreValue[5].getanimCount() + 1));
		if(scoreValue[5].getanimCount() > scoreValue[5].getanimDelay())
		{
			player.incScore();
			scoreValue[5].setanimCount(0);

			scoreValue[5].setCframe((scoreValue[5].getCframe() + 1));
			if(scoreValue[5].getCframe() > scoreValue[5].getLframe())
			{
				scoreValue[5].setCframe(0);
					
				scoreValue[4].setCframe((scoreValue[4].getCframe() + 1));
				if(scoreValue[4].getCframe() > scoreValue[4].getLframe())
				{
					scoreValue[4].setCframe(0);

					scoreValue[3].setCframe((scoreValue[3].getCframe() + 1));
					if(scoreValue[3].getCframe() > scoreValue[3].getLframe())
					{
						scoreValue[3].setCframe(0);

						scoreValue[2].setCframe((scoreValue[2].getCframe() + 1));
						if(scoreValue[2].getCframe() > scoreValue[2].getLframe())
						{
							scoreValue[2].setCframe(0);

							scoreValue[1].setCframe((scoreValue[1].getCframe() + 1));
							if(scoreValue[1].getCframe() > scoreValue[1].getLframe())
							{
								scoreValue[1].setCframe(0);

								scoreValue[0].setCframe((scoreValue[0].getCframe() + 1));
								if(scoreValue[0].getCframe() > scoreValue[0].getLframe())
								{
									scoreValue[0].setCframe(9);
								}
							}
						}
					}
				}
			}
		}

		for(n = 0; n < 4; n++)
		{
			if(collide == false && Collision(enemy[n], player))
			{
				e = n;
				player.loseLife();
				PlaySound(crash);

				collide = true; 
			}

			if(Collision(enemyPwr[n], player))
			{
				player.loseLife();
				PlaySound(hit);

				enemy[n].setFire(false);
				enemyPwr[n].setX(-50);
				enemyPwr[n].setY(-50);
				enemy[n].setShoot(true);
			}

			for(s = 0; s < 3; s++)
			{
				if(Collision(firePwr[s], enemy[n]))
				{
					enemy[n].setLife(false);
					PlaySound(hit);

					player.addScore(50);
					scoreValue[4].setCframe((scoreValue[4].getCframe() + 5));
					if(scoreValue[4].getCframe() > scoreValue[4].getLframe())
					{
						scoreValue[4].setCframe((scoreValue[4].getCframe() + 0) % 10);

						scoreValue[3].setCframe((scoreValue[3].getCframe() + 1));
						if(scoreValue[3].getCframe() > scoreValue[3].getLframe())
						{
							scoreValue[3].setCframe(0);

							scoreValue[2].setCframe((scoreValue[2].getCframe() + 1));
							if(scoreValue[2].getCframe() > scoreValue[2].getLframe())
							{
								scoreValue[2].setCframe(0);

								scoreValue[1].setCframe((scoreValue[1].getCframe() + 1));
								if(scoreValue[1].getCframe() > scoreValue[1].getLframe())
								{
									scoreValue[1].setCframe(0);

									scoreValue[0].setCframe((scoreValue[0].getCframe() + 1));
									if(scoreValue[0].getCframe() > scoreValue[0].getLframe())
									{
										scoreValue[0].setCframe(9);
									}
								}
							}
						}
					}

					enemy[n].setY(0);
					enemy[n].setX(0);
					player.endFire(s);
					firePwr[s].setX(-100);
					firePwr[s].setY(-100);
					player.letShoot(s);
				}
			}
		}
		if(!(Collision(enemy[e], player)))
		{
			collide = false;
		}
	}


	if(d3ddev->BeginScene())
	{
		if(player.testLife() <= 0)
			stage = 4;

		if(stage == 0)
			d3ddev->StretchRect(titleBack, NULL, backbuffer, NULL, D3DTEXF_NONE);
		else if(stage == 1)
			d3ddev->StretchRect(firstBack, NULL, backbuffer, NULL, D3DTEXF_NONE);
		else if(stage == 4)
		{
			StopSound(fireS);
			d3ddev->StretchRect(endBack, NULL, backbuffer, NULL, D3DTEXF_NONE);
		}

		sprite_handler->Begin(D3DXSPRITE_ALPHABLEND);

		if(stage != 0)
		{
			position.x = (float)player.getX();
			position.y = (float)player.getY();

			RECT srcRect;
			int columns = 3;

			srcRect.left = (player.getCframe() % columns) * player.getWidth();
			srcRect.top = (player.getCframe() / columns) * player.getHeight();
			srcRect.right = srcRect.left + player.getWidth();
			srcRect.bottom = srcRect.top + player.getHeight();

			sprite_handler->Draw(player_image, &srcRect, NULL, &position, 
				D3DCOLOR_XRGB(255,255,255));

			for(n = 0; n < 4; n++)
			{
				position.x = (float)enemy[n].getX();
				position.y = (float)enemy[n].getY();


				srcRect.left = (enemy[n].getCframe() % columns) * enemy[n].getWidth();
				srcRect.top = (enemy[n].getCframe() / columns) * enemy[n].getHeight();
				srcRect.right = srcRect.left + enemy[n].getWidth();
				srcRect.bottom = srcRect.top + enemy[n].getHeight();

				if(enemy[n].showEnemy() == true)
				{
					sprite_handler->Draw(enemy_image1, &srcRect, NULL, &position,
						D3DCOLOR_XRGB(255,255,255));
				}
			}

			position.x = (float)livesFont.getX();
			position.y = (float)livesFont.getY();

			sprite_handler->Draw(lives_image, NULL, NULL, &position,
				D3DCOLOR_XRGB(255,255,255));

			position.x = (float)scoreFont.getX();
			position.y = (float)scoreFont.getY();

			sprite_handler->Draw(score_image, NULL, NULL, &position,
				D3DCOLOR_XRGB(255,255,255));

			if(player.testLife() > 0)
			{
				position.x = (float)life[0].getX();
				position.y = (float)life[0].getY();

				sprite_handler->Draw(life_image, NULL, NULL, &position,
					D3DCOLOR_XRGB(255,255,255));
			}
			if(player.testLife() > 1)
			{
				position.x = (float)life[1].getX();
				position.y = (float)life[1].getY();

				sprite_handler->Draw(life_image, NULL, NULL, &position,
					D3DCOLOR_XRGB(255,255,255));
			}
			if(player.testLife() > 2)
			{
				position.x = (float)life[2].getX();
				position.y = (float)life[2].getY();

				sprite_handler->Draw(life_image, NULL, NULL, &position,
					D3DCOLOR_XRGB(255,255,255));
			}
			
			for(n = 0; n < 3; n++)
			{
				if(player.testFire(n) == true)
				{
					PlaySound(fireS);
					position.x = (float)firePwr[n].getX();
					position.y = (float)firePwr[n].getY();

					sprite_handler->Draw(firePwr_image, NULL, NULL, &position,
						D3DCOLOR_XRGB(255,255,255));
				}
			}

			for(n = 0; n < 4; n++)
			{
				position.x = (float)enemyPwr[n].getX();
				position.y = (float)enemyPwr[n].getY();

				srcRect.left = (enemyPwr[n].getCframe() % columns) * enemyPwr[n].getWidth();
				srcRect.top = (enemyPwr[n].getCframe() / columns) * enemyPwr[n].getHeight();
				srcRect.right = srcRect.left + enemyPwr[n].getWidth();
				srcRect.bottom = srcRect.top + enemyPwr[n].getHeight();

				if(enemy[n].testFire() == true)
				{
					sprite_handler->Draw(enemyPwr_image, &srcRect, NULL, &position,
						D3DCOLOR_XRGB(255,255,255));
				}
			}

			columns = 10;

			for(n = 0; n < 6; n++)
			{
				position.x = (float)scoreValue[n].getX();
				position.y = (float)scoreValue[n].getY();

				srcRect.left = (scoreValue[n].getCframe() % columns) * scoreValue[n].getWidth();
				srcRect.top = (scoreValue[n].getCframe() / columns) * scoreValue[n].getHeight();
				srcRect.right = srcRect.left + scoreValue[n].getWidth();
				srcRect.bottom = srcRect.top + scoreValue[n].getHeight();
			
				sprite_handler->Draw(number_image, &srcRect, NULL, &position,
					D3DCOLOR_XRGB(255,255,255));
			}
		}

		sprite_handler->End();

		d3ddev->EndScene();
	}

	d3ddev->Present(NULL, NULL, NULL, NULL);

	if(Key_Down(DIK_ESCAPE))
		PostMessage(hwnd, WM_DESTROY, 0, 0);

	if(Key_Down(DIK_RETURN) && stage == 0)
		stage = 1;
	else if(Key_Down(DIK_BACK) && stage == 4)
	{
		stage = 0;
		Restart();
	}

}

void Game_End(HWND hwnd)
{
	if(titleBack != NULL)
		titleBack->Release();

	if(firstBack != NULL)
		firstBack->Release();

	if(endBack != NULL)
		endBack->Release();

	if(player_image != NULL)
		player_image->Release();

	if(life_image != NULL)
		life_image->Release();

	if(lives_image != NULL)
		lives_image->Release();

	if(enemy_image1 != NULL)
		enemy_image1->Release();

	if(score_image != NULL)
		score_image->Release();

	if(number_image != NULL)
		number_image->Release();

	if(firePwr_image != NULL)
		firePwr_image->Release();

	if(enemyPwr_image != NULL)
		enemyPwr_image->Release();

	if(music_back != NULL)
		delete music_back;

	if(crash != NULL)
		delete crash;

	if(hit != NULL)
		delete hit;

	if(fireS != NULL)
		delete fireS;
}
