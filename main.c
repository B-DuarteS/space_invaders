#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>



/****************VARIÁVEIS GLOBAIS****************/

const float FPS = 120;

const int TELA_Y = 1000; // 800 pixels na vertical
const int TELA_X = 1200; // 1200 pixels na horizontal

const int GRASS_Y = 60; // barra no fim da tela que delimita onde os aliens vão chegar

const int NAVE_H = 50;
const int NAVE_L = 100;
const int NAVE_VEL = 7;

const float TIRO_NAVE_VEL = 13;
const int TIRO_L = 5;
const int TIRO_H = 20;

// LINHAS E COLUNAS TAMBÉM TEM QUE SER ALTERADAS PARA MUDAR A QUANTIDADE DE ALIENS
const int ALIENS_INIMIGOS = 30;
const int LINHAS = 5;
const int COLUNAS = 6;
const int ALIEN_L = 60;
const int ALIEN_H = 30;
const float ALIEN_X_VEL = 0.5;
const int ESPACO_X_ENTRE_ALIENS = 90; //GARANTE OS 30 PIXELS
const int ESPACO_Y_ENTRE_ALIENS = 60; // GARANTE OS 30 PIXELS

const float AUMENTO_DA_DIFICULDADE_VEL_ALEIN = 0.5; // a taxa de aumento da velocidade dos aliens a cada rodada
const float AUMENTO_DIFICULDADE_VEL_TIRO_ALIEN = 0.5; // a taxa de aumento da velocidade da bala dos aliens a cada rodada
const int MAX_TIROS_POR_ALIENS = 10; // quantos tiros os aliens podem dar em um rand
const int PROBABILIDADE_ALIEN_ATIRAR = 150; // está em 0.6% de algum alien atirar!

// VARIÁVEIS QUE MEXEM COM O TAMANHO DA FONTE, ETC!
const int ALGARISMOS_PONTOS = 50;
const int LETRA_ENORME = 90;
const int LETRA_GRNADE = 56;
const int LETRA_MEDIA = 25;
const int LETRA_PEQUENA = 18;

/**/

/**************CRIANDO CENÁRIO********************/

void desenha_cenario(ALLEGRO_BITMAP *fundo)
{
	al_draw_scaled_bitmap(fundo,
								0, 0,                                
								al_get_bitmap_width(fundo),
								al_get_bitmap_height(fundo),
								0, 0,                               
								TELA_X, TELA_Y,                      
								0);   

	al_draw_filled_rectangle( 0, TELA_Y - 20, //deixei fininho pra não atrapalhar o fundo que escolhi, mas a hitbox dele se mantém no padrão do GRASS_Y
							 TELA_X, TELA_Y, 
							 al_map_rgb(0,0,6));

}
/**/

/********************* CRIANDO NAVE ***************************/

typedef struct nave{
	float x;
	float vel;
	ALLEGRO_COLOR cor;
	ALLEGRO_BITMAP *sprite;

}Nave_t;

void inicializa_nave(Nave_t *nave)
{
	nave->x = TELA_X/2;
	nave->vel = NAVE_VEL;

	nave->sprite = al_load_bitmap("imagens/nave_modelo.png");
    if (!nave->sprite) 
	{
        fprintf(stderr, "erro ao carregar o png\n");
        exit(1);
    }
}

void desenha_nave(Nave_t nave)
{
	float y_base = TELA_Y - GRASS_Y/2;

	float largura_sprite = al_get_bitmap_width(nave.sprite);
	float altura_sprite = al_get_bitmap_height(nave.sprite);

	// Redimensiona o sprite para o hitbox do triângulo!
	al_draw_scaled_bitmap(
		nave.sprite,
		0, 0, largura_sprite, altura_sprite,  // fonte (imagem)
		nave.x - NAVE_L / 2 - 10, y_base - NAVE_H, // onde o sprite vai se encaixar na tela 
		NAVE_L + 40, NAVE_H + 30,    //assim posso definir melhor o tamanho da nave sem prejudicar a hitbox
		0);

}

void movimenta_nave(Nave_t *nave, int esquerda, int direita)
{
    if (esquerda)
        nave->x -= nave->vel;
    if (direita)
        nave->x += nave->vel;
}

//faz o alien voltar no lado oposto a extremidade que ele alcançou
void restringe_nave_na_tela(Nave_t *nave)
{
	if(nave->x > TELA_X - 1)
	{
		nave->x = 2;
	}
	if(nave->x < 1)
	{
		nave->x = TELA_X + 1;
	}
}
/**/

/********************* CRIANDO ALIEN ***************************/

typedef struct alien {
		float x, y;
		float x_vel, y_vel;
		ALLEGRO_BITMAP *sprite;
		int vida;
		

} Alien_t;

// INICIALIZA O ALIENS NA TELA
void inicializa_alien(Alien_t *alien, float velocidade) //por ter feito um vetor eu precisei fazer com que tivesse um comportamento artificial de matriz!
{
	for(int i = 0; i < LINHAS; i++)
	{	for(int j = 0; j < COLUNAS; j++) // posiciona 
		{
			int elemento = i * COLUNAS + j;
			alien[elemento].x = 0 + j * ESPACO_X_ENTRE_ALIENS;
			alien[elemento].y = 40 + i * ESPACO_Y_ENTRE_ALIENS;
			alien[elemento].x_vel = ALIEN_X_VEL + velocidade;
			alien[elemento].y_vel = ALIEN_H;
			alien[elemento].vida = 1;
			alien[elemento].sprite = al_load_bitmap("imagens/nave_alien_modelo.png"); // GARANTIR QUE SEJA ALERTADO DO ERRO!
										if (!alien[elemento].sprite) 
										{
											fprintf(stderr, "erro ao carregar o png\n");
											exit(1);
										};
		}
	}	
}

// DESENHO DO ALIEN
void desenha_aliens(Alien_t *alien, int tamanho)
{
	for(int i = 0; i < tamanho; i++)
	{
		if(alien[i].vida == 1){
				float largura_sprite = al_get_bitmap_width(alien[i].sprite);
				float altura_sprite = al_get_bitmap_height(alien[i].sprite);

			al_draw_scaled_bitmap(alien[i].sprite, 0, 0, largura_sprite, altura_sprite, 
									alien[i].x , alien[i].y ,
									ALIEN_L + 20, ALIEN_H + 20,  //ajustar a altura dos aliens
									0);
		}
	}
}
// 	MOVIMENTO DO ALIEN
void movimenta_alien_na_tela(Alien_t *alien) // se o alien estiver morto ele não executa nada nessa função
{
	int inverte_direcao = 0;
	for(int i = 0; i < LINHAS * COLUNAS; i++)
	{	
		if(alien[i].vida == 0) {continue;} // alien morto
		if(alien[i].x + ALIEN_L + alien[i].x_vel > TELA_X || alien[i].x + alien[i].x_vel < 0) // verifica se os aliens chegaram nos limites da tela
		{
			inverte_direcao = 1;
			break;
		}
	}
	if(inverte_direcao == 1) // caso tenha chegado inverte o movimento deles e desce uma fileira
	{
		for(int i = 0; i < LINHAS * COLUNAS; i++)
		{
			if(alien[i].vida == 0) {continue;} // alien morto
				alien[i].y += alien[i].y_vel;
				alien[i].x_vel *= -1;
		}			
	}

	for(int i = 0; i < LINHAS * COLUNAS; i++)
	{
		if(alien[i].vida == 0) {continue;} // alien morto
		alien[i].x += alien[i].x_vel; // movimento padrão horizontal dos aliens
	}
	
}

int verifica_aliens_vivos(Alien_t *alien, int quant_aliens)
{
	for (int i = 0; i < quant_aliens; i++) 
	{
		if(alien[i].vida != 0) {
			return 1; // ainda há alien vivo
		}
	}
	return 0; // todos mortos
}

/**/

/********************* TIRO DA NAVE ***************************/
// - ok
typedef struct tiro{ 
		float x, y;
		float y_vel;
		int mov;
		ALLEGRO_COLOR cor; 
}Tiro_t;

void inicializa_tiro_nave(Tiro_t *tiro, Nave_t nave)
{
	tiro->x = nave.x - TIRO_L/2 + 10; // + 10 é pra centralizar o tiro com a sprite
	tiro->y = TELA_Y - GRASS_Y/2 - NAVE_H;
	tiro->y_vel = TIRO_NAVE_VEL;
	tiro->mov = 1;
	tiro->cor = al_map_rgb(255,255,255);

}

//desenho do tiro da nave - ok
void desenha_tiro_nave(Tiro_t tiro)
{
	
	al_draw_filled_rectangle( tiro.x, tiro.y , 
							 tiro.x + TIRO_L, tiro.y + TIRO_H, 
							 tiro.cor);
	
}

//movimenta o tiro 
void movimenta_tiro_nave(Tiro_t *tiro, int *atirando) //atirando se refere a booleana que faz o tiro sumir ao entrar em contato com alien
{
	if (*atirando == 1)
	{
			tiro->y -= tiro->y_vel;
		if(tiro->y <= 0)
		{
			*atirando = 0;
		}
	}
}

//se chegar no fim da tela 
int verifica_tiro_passou_tela_ou_acertou_alien(Tiro_t *tiro)
{
	if(tiro->y <= 0 || tiro->mov == 0)
	{
		return 1;
	}
	return 0;
}

/********************* O TIRO DOS ALIENS ***************************/


//ALIEN
void inicializa_tiro_alien(Tiro_t *tiro_alien, int quantidade_tiros_aliens, int velocidade)
{
	for(int i = 0; i < quantidade_tiros_aliens; i++)
	{
		tiro_alien[i].x = 0;
		tiro_alien[i].y = 0;
		tiro_alien[i].y_vel = 2 + velocidade;
		tiro_alien[i].mov = 0;
		tiro_alien[i].cor = al_map_rgb(0,255,0);
	}
}

void desenha_tiro_alien(Tiro_t *tiro_alien, int quantidade)
{
	for(int i = 0; i < quantidade; i++)
	{
		if(tiro_alien[i].mov == 1)
		{ 			
			al_draw_filled_rectangle( tiro_alien[i].x, tiro_alien[i].y,
									tiro_alien[i].x + TIRO_L, tiro_alien[i].y + TIRO_H,
									tiro_alien[i].cor); 
		}
	}
}

void movimenta_tiro_alien(Tiro_t *tiro, int quantidade_tiros_aliens) // determina o movimento do tiro
{
	for(int i = 0; i < quantidade_tiros_aliens; i++)
	{
		if(tiro[i].mov == 1)
		{	
			tiro[i].y += tiro[i].y_vel;
			if(tiro[i].y >= TELA_Y)
			{
				tiro[i].mov = 0; // se passar a tela faz o tiro sumir
			}
		}
	}

}

void randomiza_tiro_alien(Tiro_t *tiro_alien, Alien_t *alien)
{
	if(rand() % PROBABILIDADE_ALIEN_ATIRAR < 1) // passo a poder controlar a chance do alien atirar com uma variável global!
	{
		for(int i = 0; i < MAX_TIROS_POR_ALIENS; i++) 
		{
			if(tiro_alien[i].mov == 0) 
			{
				int elemento = rand() % ALIENS_INIMIGOS;
				if(alien[elemento].vida == 1) 
				{
							tiro_alien[i].x = alien[elemento].x + ALIEN_L/2 - TIRO_L/2; // centralizar o eixo X do tiro
							tiro_alien[i].y = alien[elemento].y + ALIEN_H; // centralizar o eixo Y do tiro
							tiro_alien[i].mov = 1;
							break; // um tiro por vez
				}
			}
		}
	}
}
/**/

/********************* COLISÕES ***************************/

//quandos os aliens batem na nave
int colisao_alien_solo(Alien_t *alien)
{
	int teve_colisao = 0;

	for(int i = 0; i < LINHAS * COLUNAS; i++)
	{	
		if(alien[i].y > TELA_Y - GRASS_Y) // verifica se os aliens chegaram na GRASS_Y
		{
			teve_colisao = 1;
			break;
		}
	}
	if(teve_colisao == 1)
	{
		return 0;
	}
}

//quando os aliens batem na nave
int colisao_alien_com_nave(Nave_t nave, Alien_t *alien)
{
    // verifica as coodernadas do triangulo
    float nave_topo = TELA_Y - GRASS_Y/2 - NAVE_H;
    float nave_base = TELA_Y - GRASS_Y/2;
    float nave_esq  = nave.x - NAVE_L/2;
    float nave_dir  = nave.x + NAVE_L/2;

    for (int i = 0; i < LINHAS * COLUNAS; i++) {
        // limites para o alien
        float alien_topo = alien[i].y;
        float alien_base = alien[i].y + ALIEN_H;
        float alien_esq  = alien[i].x;
        float alien_dir  = alien[i].x + ALIEN_L;

        // Verifica sobreposição horizontal e vertical
        int colide_horizontal = (alien_dir >= nave_esq && alien_esq <= nave_dir);
        int colide_vertical   = (alien_base >= nave_topo && alien_topo <= nave_base);

        if (colide_horizontal && colide_vertical) {
            return 0;  // Houve colisão
        }
    }

    return 1;  // Nenhuma colisão
}

//quando o tiro colide com o alien
int colisao_tiro_nave_alien(Tiro_t *tiro, Alien_t *alien, int quant_aliens)
{
	int contador = 0;

	for (int i = 0; i < quant_aliens; i++) {
	
		if(alien[i].vida == 0) { continue; }

		//limites para o tiro
		float tiro_x1 = tiro->x;
		float tiro_y1 = tiro->y;
		float tiro_x2 = tiro->x + TIRO_L;
		float tiro_y2 = tiro->y + TIRO_H;
	
        // limites para o alien
		float alien_x1  = alien[i].x;
        float alien_y1 = alien[i].y;
		float alien_x2  = alien[i].x + ALIEN_L;
        float alien_y2 = alien[i].y + ALIEN_H;

        // Verifica sobreposição horizontal e vertical do alien com tiro
        int colide_horizontal = !(tiro_x2 < alien_x1 || tiro_x1 > alien_x2);
		int colide_vertical = !(tiro_y2 < alien_y1 || tiro_y1 > alien_y2);

		//houve colisão e mata o alien da posição i!
        if(colide_horizontal && colide_vertical) 
		{
			alien[i].vida = 0;
			tiro->mov = 0;
        	return 1; // alien morreu
        }
	}
	return 0; // alien tá vivo
}

int colisao_tiro_do_alien_com_nave(Tiro_t *tiro_alien, Nave_t nave)
{
	 // verifica as coodernadas do triangulo
    float nave_topo = TELA_Y - GRASS_Y/2 - NAVE_H + 25; // esses numeros (25 e 65) são para a hitbox ficar mais precisa com relação a nave e poder aumentar o espaço de desvio do jogador!
    float nave_base = TELA_Y - GRASS_Y/2;
    float nave_esq  = nave.x - NAVE_L/2 + 65;
    float nave_dir  = nave.x + NAVE_L/2;

    for (int i = 0; i < LINHAS * COLUNAS; i++) {
        // limites para o alien
        float tiro_alien_topo = tiro_alien[i].y;
        float tiro_alien_base = tiro_alien[i].y + ALIEN_H;
        float tiro_alien_esq  = tiro_alien[i].x;
        float tiro_alien_dir  = tiro_alien[i].x + ALIEN_L;

        // Verifica sobreposição horizontal e vertical
        int colide_horizontal = (tiro_alien_dir >= nave_esq && tiro_alien_esq <= nave_dir);
        int colide_vertical   = (tiro_alien_base >= nave_topo && tiro_alien_topo <= nave_base);

        if (colide_horizontal && colide_vertical) {
            return 0;  // Houve colisão
        }
    }

    return 1;  // Nenhuma colisão
}


/**/

/******************************** PROCESSOS DE CONSTRUÇÃO DE HUD ************************************************/

void desenha_pontuacao(int pontuacao, ALLEGRO_FONT *fonte)
{
	char texto[100];
    sprintf(texto, "Pontos: %d", pontuacao);
    al_draw_text(fonte, al_map_rgb(255, 255, 255), 20, 10, 0, texto); // distancia ajustada no canto superior  esquerdo da tela! 
}

void desenha_fase(int fase, ALLEGRO_FONT *fonte)
{
	char texto[100];
	sprintf(texto, "Fase: %d", fase);
	al_draw_text(fonte, al_map_rgb(255, 255, 255), TELA_X - 130, 10, 0, texto); // distancia ajustada no canto superior direito da tela!
}

int menu_do_jogo(ALLEGRO_FONT *fonte_grande, ALLEGRO_FONT *fonte_enorme, ALLEGRO_FONT *fonte_pequena,  ALLEGRO_BITMAP *fundo)
{
	ALLEGRO_EVENT_QUEUE *fila_eventos_menu = NULL;
    ALLEGRO_TIMER *timer_menu;
    ALLEGRO_EVENT ev_menu;
	ALLEGRO_COLOR cor_branca;
	ALLEGRO_COLOR cor_roxa;
	ALLEGRO_COLOR cor_verde;
	ALLEGRO_COLOR cor_opcao1, cor_opcao2, cor_opcao3;
	
	 			/***********ROTINAS DE INICIALIZAÇÃO****************/

	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer_menu = al_create_timer(1.0 / FPS);
    if(!timer_menu) 
	{
		fprintf(stderr, "falha para criar o timer!\n");
		return -1;
	}

	//instala o mouse
	if(!al_install_mouse()) 
	{
		fprintf(stderr, "falha ao inicializar o mouse!\n");
		return -1;
	}

			/************* CRIA INICIALIZA A FILA DE EVENTOS *********************/

	fila_eventos_menu = al_create_event_queue();
	if(!fila_eventos_menu) 
	{
		fprintf(stderr, "não conseguiu criar a event_queue!\n");
		al_destroy_timer(timer_menu);
		return -1;
	}

	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(fila_eventos_menu, al_get_keyboard_event_source());
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
    al_register_event_source(fila_eventos_menu, al_get_timer_event_source(timer_menu));
	// registram na fila eventos de mouse;
	al_register_event_source(fila_eventos_menu, al_get_display_event_source(al_get_current_display()));
    


	/*************** FUNÇÕES E PROCESSOS DE INICIALIZAÇÃO DO JOGO ****************/
	
	al_start_timer(timer_menu);
	int menu = 1; // quando 1 ele tá ativo!
	int modo = 0; // variar em 3 (0-2) 0 - iniciar o jogo 1 - recordes 2 - tutorial 
	cor_branca = al_map_rgb( 255, 255, 255);
	cor_roxa = al_map_rgb(151,8,255);
	cor_verde = al_map_rgb(0, 225, 0);

	while(menu)
	{
		al_wait_for_event(fila_eventos_menu, &ev_menu);
		//primeiro evento é conseguir controlar o teclado
		if(ev_menu.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev_menu.keyboard.keycode){

			case ALLEGRO_KEY_UP:
				if(modo > 0)
				{modo--;}  // fica movimentando nas opções do menu
			break;
			
			case ALLEGRO_KEY_DOWN:
				if(modo < 2)
				{modo++;} //  fica movimentando nas opções do menu
			break;
			
			case ALLEGRO_KEY_ENTER:
				menu = 0; // sai do menu
			break;
			}
		}

		//fechamento da tela quando clica no x
		if(ev_menu.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			menu = 0;
		}
		
		al_draw_scaled_bitmap(fundo,
								0, 0,                                
								al_get_bitmap_width(fundo),
								al_get_bitmap_height(fundo),
								0, 0,                               
								TELA_X, TELA_Y,                      
								0); 

		//esses if's determinam a qual cor e qual menu setado, se roxo então é o menu desejado se branco então não está selecionado
		if (modo == 0)
			{ cor_opcao1 = cor_roxa; }
		else
			{ cor_opcao1 = cor_branca; }

		if (modo == 1)
			{ cor_opcao2 = cor_roxa; }
		else
			{ cor_opcao2 = cor_branca; }

		if (modo == 2)
			{ cor_opcao3 = cor_roxa; }
		else
			{ cor_opcao3 = cor_branca; }

		al_draw_text(fonte_grande, cor_opcao1, TELA_X/2 - 300, TELA_Y/2 - 40, 0, "MODO NORMAL");
		al_draw_text(fonte_grande, cor_opcao2, TELA_X/2 - 250, TELA_Y/2 + 40, 0, "RECORDES"); // modo desafio sinistro
		al_draw_text(fonte_grande, cor_opcao3, TELA_X/2 - 230, TELA_Y/2 + 120, 0, "TUTORIAL");
		al_draw_text(fonte_enorme, cor_roxa, TELA_X/2 - 560, TELA_Y/2 - 260, 0, "SPACE INVADERS" ); // colocar o space invaders no centro da tela
		al_draw_text(fonte_pequena, cor_verde, TELA_X/2 - 400, TELA_Y - 210, 0, "uses as setas cima e baixo e o enter para selecionar o modo!" ); // coloca o guia pro jogador 


		// atualiza tela
		al_flip_display();

	}

	// fecha a tela, limpa a memoria, etc.
	al_destroy_event_queue(fila_eventos_menu);
	al_destroy_timer(timer_menu);

	return modo;
}


// TELA DE TUTORIAL DO JOGO 
int modo_tutorial( ALLEGRO_FONT *fonte_auxiliar, ALLEGRO_FONT *fonte_enorme, ALLEGRO_BITMAP *fundo_astronauta)
{
	ALLEGRO_EVENT_QUEUE *fila_eventos_tutorial = NULL;
    ALLEGRO_TIMER *timer_tutorial;
    ALLEGRO_EVENT ev_tutorial;
	ALLEGRO_COLOR cor_roxa;
	ALLEGRO_COLOR cor_branca;
	
	 			/***********ROTINAS DE INICIALIZAÇÃO****************/

	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer_tutorial = al_create_timer(1.0 / FPS);
    if(!timer_tutorial) 
	{
		fprintf(stderr, "falha para criar o timer!\n");
		return -1;
	}

	//instala o mouse
	if(!al_install_mouse()) 
	{
		fprintf(stderr, "falha ao inicializar o mouse!\n");
		return -1;
	}
	
			/************* CRIA INICIALIZA A FILA DE EVENTOS *********************/

	fila_eventos_tutorial = al_create_event_queue();
	if(!fila_eventos_tutorial) 
	{
		fprintf(stderr, "não conseguiu criar a event_queue!\n");
		al_destroy_timer(timer_tutorial);
		return -1;
	}

	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(fila_eventos_tutorial, al_get_keyboard_event_source());
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
    al_register_event_source(fila_eventos_tutorial, al_get_timer_event_source(timer_tutorial));
	// registram na fila eventos de mouse;
	al_register_event_source(fila_eventos_tutorial, al_get_display_event_source(al_get_current_display()));


	/*************** FUNÇÕES E PROCESSOS DE INICIALIZAÇÃO DO JOGO ****************/
	
	al_start_timer(timer_tutorial);
	int tutorial = 1; // quando 1 ele tá ativo!
	cor_roxa = al_map_rgb(151,8,255);
	cor_branca = al_map_rgb(255,255,255);

	while(tutorial)
	{
		al_wait_for_event(fila_eventos_tutorial, &ev_tutorial);
		//primeiro evento é conseguir controlar o teclado
		if(ev_tutorial.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev_tutorial.keyboard.keycode){
			
			case ALLEGRO_KEY_ENTER:
				tutorial = 0; // sai do menu
			break;
			}
		}
		//fechamento da tela quando clica no x
		if(ev_tutorial.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			tutorial = 0;
		}


				al_draw_scaled_bitmap(fundo_astronauta,
								0, 0,                                
								al_get_bitmap_width(fundo_astronauta),
								al_get_bitmap_height(fundo_astronauta),
								0, 0,                               
								TELA_X, TELA_Y,                      
								0); 

		al_draw_text(fonte_enorme, cor_roxa, TELA_X/2 - 570, TELA_Y/2 - 300, 0, "UNA-SE A CAUSA");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 150, 0, "Pressione 'A' para mover a nave para a esquerda");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 110, 0, "Pressione 'D' para mover a nave para a direita");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 70, 0, "Pressione 'SPACE' para atirar");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 30, 0, "Boa sorte na guerra contra os aliens, patrulheiro!");
		al_draw_text(fonte_auxiliar, cor_roxa, TELA_X/2 - 400, TELA_Y/2 + 80, 0, "Pressione ENTER para iniciar o combate");

		al_flip_display();
	}

	// fecha a tela, limpa a memoria, etc.
	al_destroy_event_queue(fila_eventos_tutorial);
	al_destroy_timer(timer_tutorial);
	return 0;
}

/*****************SISTEMA DE LEITURA E ARMAZENAMENTO DO RECORDE ***************************/

void salvamento_dos_pontos(int pontos)
{
	int recorde;
	FILE* arquivo = fopen("recorde/recorde.txt", "r");
	if(arquivo == NULL)
	{
		printf("erro ao executar o arquivo");
	}

	fscanf(arquivo, "%d", &recorde); // escreve o valor do arquivo na variável recorde!

	if(pontos > recorde) //compara o valor da variavel recorde com os pontos feitos pelo jogador!
	{
		fclose(arquivo);
		arquivo = fopen("recorde/recorde.txt", "w");
		fprintf(arquivo, "%d", pontos); // escreve valor de pontos no recorde txt
	}
	fclose(arquivo);
}

char *mostra_recorde()
{
	int recorde;
	FILE* arquivo = fopen("recorde/recorde.txt", "r");
	if(arquivo == NULL)
	{
		printf("erro ao executar o arquivo");
	}

	fscanf(arquivo, "%d", &recorde); // escreve o valor do arquivo na variável recorde!
	char *texto = malloc(sizeof(char) * ALGARISMOS_PONTOS); // algarismos_pontos define quantos algarismos o numero de pontos do jogador pode ter
	sprintf(texto, "%d", recorde); // transforma o recorde de int pra char
	return texto;
}

/****************/


// MENU DO RECORDE
int menu_recorde( ALLEGRO_FONT *fonte_auxiliar, ALLEGRO_FONT *fonte_enorme, ALLEGRO_FONT *fonte_grande, ALLEGRO_BITMAP *fundo_espaco) // SO FALTA CONFIGURAR AS FONTES!
{
	ALLEGRO_EVENT_QUEUE *fial_eventos_recorde = NULL;
    ALLEGRO_TIMER *timer_recorde;
    ALLEGRO_EVENT ev_recorde;
	ALLEGRO_COLOR cor_roxa;
	ALLEGRO_COLOR cor_rosa;
	ALLEGRO_COLOR cor_branca;
	ALLEGRO_COLOR cor_vermelha;
	
	 			/***********ROTINAS DE INICIALIZAÇÃO****************/

	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer_recorde = al_create_timer(1.0 / FPS);
    if(!timer_recorde) 
	{
		fprintf(stderr, "falha para criar o timer!\n");
		return -1;
	}

	//instala o mouse
	if(!al_install_mouse()) 
	{
		fprintf(stderr, "falha ao inicializar o mouse!\n");
		return -1;
	}
	
			/************* CRIA INICIALIZA A FILA DE EVENTOS *********************/

	fial_eventos_recorde = al_create_event_queue();
	if(!fial_eventos_recorde) 
	{
		fprintf(stderr, "não conseguiu criar a event_queue!\n");
		al_destroy_timer(timer_recorde);
		return -1;
	}

	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(fial_eventos_recorde, al_get_keyboard_event_source());
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
    al_register_event_source(fial_eventos_recorde, al_get_timer_event_source(timer_recorde));
	// registram na fila eventos de mouse;
	al_register_event_source(fial_eventos_recorde, al_get_display_event_source(al_get_current_display()));


	/*************** FUNÇÕES E PROCESSOS DE INICIALIZAÇÃO DO JOGO ****************/
	
	al_start_timer(timer_recorde);
	int recorde = 1; // quando 1 ele tá ativo!
	char *valor_recorde = mostra_recorde();
	cor_roxa = al_map_rgb(151,8,255);
	cor_rosa = al_map_rgb(255, 4, 255);
	cor_branca = al_map_rgb( 255, 255, 255);
	cor_vermelha = al_map_rgb(136, 2, 2);

	while(recorde)
	{
		al_wait_for_event(fial_eventos_recorde, &ev_recorde);
		//primeiro evento é conseguir controlar o teclado
		if(ev_recorde.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev_recorde.keyboard.keycode){
			
			case ALLEGRO_KEY_ENTER:
				recorde = 0; // sai do menu
			break;
			}
		}
		//fechamento da tela quando clica no x
		if(ev_recorde.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			recorde = 0;
		}


				al_draw_scaled_bitmap(fundo_espaco,
								0, 0,                                
								al_get_bitmap_width(fundo_espaco),
								al_get_bitmap_height(fundo_espaco),
								0, 0,                               
								TELA_X, TELA_Y,                      
								0); 

		al_draw_text(fonte_grande, cor_roxa, TELA_X/2 - 550, TELA_Y/2 - 300, 0, "O GRANDE PATRULHEIRO!");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 150, 0, "ninguem sabe sua real identidade, somente de");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 110, 0, "seus grandes feitos em combates que perpetuam ");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 70, 0, "pela infinidade do cosmo subjulgando os horripilantes");
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 500, TELA_Y/2 - 30, 0, "aliens, entre nessa jornada e prove seu valor!! ");
		al_draw_text(fonte_enorme, cor_vermelha, TELA_X/2 - 200, TELA_Y/2 + 80, 0, valor_recorde);
		al_draw_text(fonte_auxiliar, cor_branca, TELA_X/2 - 550, TELA_Y/2 + 220, 0, "Pressione ENTER e busque a fama entre as estrelas!!!");

		al_flip_display();
	}

	// fecha a tela, limpa a memoria, etc.
	al_destroy_event_queue(fial_eventos_recorde);
	al_destroy_timer(timer_recorde);
	return 0;

}

/********/


int main(int argc, char **argv) 
{

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT ev;
	ALLEGRO_FONT *fonte = NULL;
	ALLEGRO_FONT *fonte_grande_menu = NULL;
	ALLEGRO_FONT *fonte_enorme_menu = NULL;
	ALLEGRO_FONT *fonte_auxiliar_menu = NULL;
	ALLEGRO_BITMAP *fundo = NULL;
	ALLEGRO_BITMAP *fundo_astronauta = NULL;

	/****************ROTINAS DE INICIALIZAÇÃO****************/

	//inicializa o Allegro - verificarr se houve algum erro
	if(!al_init()) 
	{
		fprintf(stderr, "alegro não inicializou!\n");
		return -1;
	}
   
	 //inicializa o módulo de primitivas
    if(!al_init_primitives_addon())
	{
		fprintf(stderr, "falha para inicializar as primitivas!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon())
	{
		fprintf(stderr, "falha oa criar o modulo de imagem!\n");
		return -1;
	}
   
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) 
	{
		fprintf(stderr, "falha para criar o timer!\n");
		return -1;
	}

	//cria uma tela com dimensoes de TELA_LxTELA_H pixels
	display = al_create_display(TELA_X, TELA_Y);
	if(!display) 
	{
		fprintf(stderr, "falha para criar o display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if(!al_install_keyboard())
	{
		fprintf(stderr, "falha ao inicializar o keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) 
	{
		fprintf(stderr, "falha ao inicializar o mouse!\n");
		return -1;
	}

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	if (!al_init_ttf_addon()) {
		fprintf(stderr, "falha ao inicializar o modulo .ttf!\n");
		return -1;
	}
	
	// Carrega a fonte do modo normal
	fonte = al_load_font("fonte/04B_30__.TTF", LETRA_PEQUENA , 0);
	if (!fonte) {
		fprintf(stderr, "falha ao carregar a fonte!\n");
		return -1;
	}

	fonte_auxiliar_menu = al_load_font("fonte/04B_30__.TTF", LETRA_MEDIA , 0);
	if (!fonte_auxiliar_menu) {
    fprintf(stderr, "falha ao carregar fonte do menu!\n");
    return -1;
	}

	//carrega a fonte dos menus
	fonte_grande_menu = al_load_font("fonte/04B_30__.TTF", LETRA_GRNADE , 0);
	if (!fonte_grande_menu) {
    fprintf(stderr, "falha ao carregar fonte do menu!\n");
    return -1;
	}

	//carrega a fonte enorme dos menu
	fonte_enorme_menu = al_load_font("fonte/04B_30__.TTF", LETRA_ENORME , 0);
	if (!fonte_enorme_menu) {
    fprintf(stderr, "falha ao carregar fonte do menu!\n");
    return -1;
	}

	fundo = al_load_bitmap("imagens/fundo_invaders.png");
	if (!fundo) 
	{
		fprintf(stderr, "falha ao carregar imagem de fundo!\n");
		return -1;
	}

	fundo_astronauta = al_load_bitmap("imagens/astronauta_incentivando.png");
	if (!fundo_astronauta) 
	{
		fprintf(stderr, "falha ao carregar imagem de fundo!\n");
		return -1;
	}

	/**/


 	/********************* CRIA INICIALIZA A FILA DE EVENTOS ***************************/

	fila_eventos = al_create_event_queue();
	if(!fila_eventos) 
	{
		fprintf(stderr, "não conseguiu criar a event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(fila_eventos, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(fila_eventos, al_get_mouse_event_source());  	
	
	/**/


	/*************** FUNÇÕES E PROCESSOS DE INICIALIZAÇÃO DO JOGO ****************/


	int modo = menu_do_jogo(fonte_grande_menu, fonte_enorme_menu, fonte, fundo);
	// tudo que se refere ao menu principal do jogo
	if(modo == 0 || modo == 2 || modo == 1) // poderia ter feito uma função mas daria mais trabalho do que o necessário!
	{
		if(modo == 1)
		{
			menu_recorde(fonte_auxiliar_menu, fonte_enorme_menu, fonte_grande_menu, fundo); // fundo é o fundo espacial!
		}

		if(modo == 2) //ao escolher o tutorial no menu automaticamente iniciará uma partida
		{
			modo_tutorial(fonte_auxiliar_menu, fonte_enorme_menu, fundo_astronauta);
		}


		al_start_timer(timer);
		//movimentação da nave
		int direita = 0;
		int esquerda = 0;
		//tiro da nave
		int atira = 0;
		// Inicialização da nave
		Nave_t nave;
		Tiro_t tiro_nave;
		inicializa_nave(&nave);


		// inicialização dos aliens
		Alien_t *alien = malloc(sizeof(Alien_t)* ALIENS_INIMIGOS); // garantir o processo de criação dos aliens
		Tiro_t *tiro_alien = malloc(sizeof(Tiro_t)* MAX_TIROS_POR_ALIENS); //garantir que cada alien possa atirar
		float velocidade_alien = 0; // vou usar pra aumentar velocidade dos aliens a cada fase!
		float velocidade_bala_alien = 0; // vou usar pra aumentar velocidade da bala dos aliens a cada fase!
		inicializa_alien(alien, velocidade_alien);
		inicializa_tiro_alien(tiro_alien, MAX_TIROS_POR_ALIENS, velocidade_bala_alien);

		//variáveis responsáveis por acúmulo de pontos, fases, etc
		int fase = 1; // variável que controla a fase do jogo
		int pontuacao = 0;

		int jogando = 1;
		while(jogando)
		{
			al_wait_for_event(fila_eventos, &ev);

			
			if(ev.type == ALLEGRO_EVENT_TIMER)
			{
				movimenta_nave(&nave, esquerda, direita);

				desenha_cenario(fundo); // inicializando cenário

				desenha_pontuacao(pontuacao, fonte);

				desenha_fase(fase, fonte);

				desenha_nave(nave); // inicializa a nave no cenário

				desenha_aliens(alien, ALIENS_INIMIGOS); // inicializa o alien no cenário

				// faz o processo correto para atirar
				if(atira == 1)
				{
					desenha_tiro_nave(tiro_nave);
					movimenta_tiro_nave(&tiro_nave, &atira);

					if( colisao_tiro_nave_alien(&tiro_nave, alien, ALIENS_INIMIGOS))
					{
						pontuacao += 10;
					}
					if ( colisao_tiro_nave_alien(&tiro_nave, alien, ALIENS_INIMIGOS) || verifica_tiro_passou_tela_ou_acertou_alien(&tiro_nave))
					{
						atira = 0;
						
					}
				}

				movimenta_alien_na_tela(alien);

				randomiza_tiro_alien(tiro_alien, alien);
				
				desenha_tiro_alien(tiro_alien, MAX_TIROS_POR_ALIENS);
				movimenta_tiro_alien(tiro_alien, MAX_TIROS_POR_ALIENS);

				if(verifica_aliens_vivos(alien, ALIENS_INIMIGOS) == 0) // faz a dinamica de aumento da dificuldade das fases! (aumento da velocidade dos aliens e de seus disparos)
				{
					velocidade_alien += AUMENTO_DA_DIFICULDADE_VEL_ALEIN;
					inicializa_alien(alien, velocidade_alien);
					
					velocidade_bala_alien += AUMENTO_DIFICULDADE_VEL_TIRO_ALIEN;
					inicializa_tiro_alien(tiro_alien, MAX_TIROS_POR_ALIENS, velocidade_bala_alien);

					
					fase++; // aumenta a fase do jogo
					pontuacao += 50; // aumenta a pontuação do jogador	
				}


				if(colisao_alien_solo(alien) == 0 || colisao_alien_com_nave(nave, alien) == 0 || colisao_tiro_do_alien_com_nave(tiro_alien, nave) == 0)
				{
					jogando = 0;
				}

				restringe_nave_na_tela(&nave); //faz com que a nave não transpace o limite da tela


				al_flip_display();

				if(al_get_timer_count(timer)%(int)FPS == 0)
				{
					printf("\n%d segundos se passaram", (int)(al_get_timer_count(timer)/FPS));
					al_flip_display();
				}
			}

			/*************** CONTROLES DA NAVE NA TELA ****************/
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(ev.keyboard.keycode){

				case ALLEGRO_KEY_A:
					esquerda = 1; // fica movimentando a nave na tela
				break;
				
				case ALLEGRO_KEY_D:
					direita = 1; // fica movimentando a nave na tela
				break;
				}
			}

			if(ev.type == ALLEGRO_EVENT_KEY_UP) // garante o movimento continuo pressionando a tecla
			{
				switch(ev.keyboard.keycode){
					case ALLEGRO_KEY_A:
						esquerda = 0;
					break;

					case ALLEGRO_KEY_D:
						direita = 0;
					break;
				}
			}

			// tecla para o tiro
			if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				switch(ev.keyboard.keycode)
				{
					case ALLEGRO_KEY_SPACE:
						if(atira == 0)
							{ 	
								atira = 1;
								inicializa_tiro_nave(&tiro_nave, nave);
							}
						
					break;
				}
			}



			//fechamento da tela quando clica no x
			if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				jogando = 0;
			}

		}
		

		/****************PROCEDIMENTOS DE FIM DE JOGO****************/
		
		salvamento_dos_pontos(pontuacao);

		// fecha a tela, limpa a memoria, etc.
		al_destroy_timer(timer);
		al_destroy_display(display);
		al_destroy_event_queue(fila_eventos);
		al_destroy_font(fonte);
		al_destroy_bitmap(fundo);
		al_destroy_bitmap(nave.sprite);
		for(int i = 0; i < ALIENS_INIMIGOS; i++)
		{
			al_destroy_bitmap(alien[i].sprite);
		}
		free(alien);
		free(tiro_alien);
	}

 
	return 0;
}
