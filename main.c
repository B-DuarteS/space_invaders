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

const int TELA_Y = 900; // 900 pixels na vertical
const int TELA_X = 1200; // 1200 pixels na horizontal

const int GRASS_Y = 60; // barra no fim da tela que delimita onde os aliens vão chegar

const int NAVE_H = 50;
const int NAVE_L = 100;
const int NAVE_VEL = 7;

const float TIRO_NAVE_VEL = 13;
const int TIRO_L = 5;
const int TIRO_H = 20;

// LINHAS E COLUNAS TAMBÉM TEM QUE SER ALTERADAS PARA MUDAR A QUANTIDADE DE ALIENS
const int ALIENS_INIMIGOS = 40;
const int LINHAS = 4;
const int COLUNAS = 10;
const int ALIEN_L = 60;
const int ALIEN_H = 30;
const float ALIEN_X_VEL = 0.5;
const float AUMENTO_DA_DIFICULDADE_VEL_ALEIN = 0.5; // a taxa de aumento da velocidade dos aliens a cada rodada
const float AUMENTO_DIFICULDADE_VEL_TIRO_ALIEN = 0.5; // a taxa de aumento da velocidade da bala dos aliens a cada rodada
const int PROBABILIDADE_ALIEN_ATIRAR = 150; // está em 0.6% de algum alien atirar!
const int ESPACO_X_ENTRE_ALIENS = 90;
const int ESPACO_Y_ENTRE_ALIENS = 60;

const int MAX_TIROS_POR_ALIENS = 10; // quantos tiros os aliens podem dar em um rand



/**/

/**************CRIANDO CENÁRIO********************/

void desenha_cenario()
{
	al_clear_to_color(al_map_rgb(0,0,0)); // limpar tela com preto
	al_draw_filled_rectangle( 0, TELA_Y - GRASS_Y, 
							 TELA_X, TELA_Y, 
							 al_map_rgb(0,0,36));

}
/**/

/********************* CRIANDO NAVE ***************************/

typedef struct nave{
	float x;
	float vel;
	ALLEGRO_COLOR cor;

} Nave_t;

void inicializa_nave(Nave_t *nave)
{
	nave->x = TELA_X/2;
	nave->vel = NAVE_VEL;
	nave->cor = al_map_rgb(151,8,255);
}

void desenha_nave(Nave_t nave)
{
	float y_base = TELA_Y - GRASS_Y/2;
	al_draw_filled_triangle( nave.x, y_base - NAVE_H, 
							 nave.x - NAVE_L/2, y_base,
							 nave.x + NAVE_L/2, y_base,
							 nave.cor);
	

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
		ALLEGRO_COLOR cor;
		int vida;

} Alien_t;

// INICIALIZA O ALIENS NA TELA
void inicializa_alien(Alien_t *alien, float velocidade)
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
			alien[elemento].cor = al_map_rgb(rand()%256, rand()%256, rand()%256 );
		}
	}	
}

// DESENHO DO ALIEN
void desenha_aliens(Alien_t *alien, int tamanho)
{
	for(int i = 0; i < tamanho; i++)
	{
		if(alien[i].vida == 1){
			al_draw_filled_rectangle( alien[i].x , alien[i].y ,
									alien[i].x + ALIEN_L , alien[i].y + ALIEN_H ,
									alien[i].cor);
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
	tiro->x = nave.x - TIRO_L/2;
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
		tiro_alien[i].cor = al_map_rgb(255,0,0);
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

int menu_do_jogo(ALLEGRO_FONT *fonte_grande, ALLEGRO_FONT *fonte_enorme, ALLEGRO_FONT *fonte_pequena)
{
	ALLEGRO_EVENT_QUEUE *fila_eventos_menu = NULL;
    ALLEGRO_TIMER *timer_menu = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT ev_menu;
	ALLEGRO_COLOR cor_roxa;
	
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
	int modo = 0; // variar em 4 (0-3) 0 - iniciar o jogo 1 - recordes 2 - tutorial 3 - modo HARDCORE!
	cor_roxa = al_map_rgb(151,8,255);

	while(menu)
	{
		al_wait_for_event(fila_eventos_menu, &ev_menu);
		//primeiro evento é conseguir controlar o teclado
		if(ev_menu.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			switch(ev_menu.keyboard.keycode){

			case ALLEGRO_KEY_ENTER:
				menu = 0; // sai do menu
			break;
			}
		}

		//fechamento da tela quando clica no x
		if(ev_menu.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			menu = 0;
		}
		
		al_clear_to_color(al_map_rgb(0, 0, 0));

		al_draw_text(fonte_grande, cor_roxa, TELA_X/2 - 200, TELA_Y/2, 0, "INICIAR");
		al_draw_text(fonte_enorme, cor_roxa, TELA_X/2 - 560, TELA_Y/2 - 260, 0, "SPACE INVADERS" ); // colocar o space invaders no centro da tela
		al_draw_text(fonte_pequena, cor_roxa, TELA_X/2 - 240, TELA_Y - 180, 0, "pressione ENTER para iniciar" ); // coloca o guia pro jogador 
		al_draw_text(fonte_pequena, cor_roxa, TELA_X/2 - 440, TELA_Y - 360, 0, "Use 'A' e 'D' para se movimentar e 'SPACE' para atirar" );


		// atualiza tela
		al_flip_display();

	}

	// fecha a tela, limpa a memoria, etc.
	al_destroy_event_queue(fila_eventos_menu);
	al_destroy_timer(timer_menu);

	return modo;
}


/****/


int main(int argc, char **argv) 
{

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT ev;
	ALLEGRO_FONT *fonte = NULL;
	ALLEGRO_FONT *fonte_grande_menu = NULL;
	ALLEGRO_FONT *fonte_enorme_menu = NULL;

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
	
	// Carrega a fonte do modo normal e pequena no menu
	fonte = al_load_font("fonte/04B_30__.TTF", 18 , 0);
	if (!fonte) {
		fprintf(stderr, "falha ao carregar a fonte!\n");
		return -1;
	}

	//carrega a fonte grande do menu
	fonte_grande_menu = al_load_font("fonte/04B_30__.TTF", 56 , 0);
	if (!fonte_grande_menu) {
    fprintf(stderr, "Falha ao carregar fonte do menu!\n");
    return -1;
	}

	//carrega a fonte enorme dos menu
	fonte_enorme_menu = al_load_font("fonte/04B_30__.TTF", 90 , 0);
	if (!fonte_enorme_menu) {
    fprintf(stderr, "Falha ao carregar fonte do menu!\n");
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



	// tudo que se refere ao menu principal do jogo
	menu_do_jogo(fonte_grande_menu, fonte_enorme_menu, fonte);

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

			desenha_cenario(); // inicializando cenário

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
				velocidade_bala_alien += AUMENTO_DIFICULDADE_VEL_TIRO_ALIEN;
				inicializa_tiro_alien(tiro_alien, MAX_TIROS_POR_ALIENS, velocidade_bala_alien);

				velocidade_alien += AUMENTO_DA_DIFICULDADE_VEL_ALEIN;
				inicializa_alien(alien, velocidade_alien);
				
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
	
	// fecha a tela, limpa a memoria, etc.
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(fila_eventos);
	al_destroy_font(fonte);
	free(alien);
	free(tiro_alien);
	
 
	return 0;
}
