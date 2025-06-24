#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

// ideia: fazer um vetor e colocar todos os aliens nele pra (são 55 aliens independente da fase)


/****************VARIÁVEIS GLOBAIS****************/

const float FPS = 120;

const int TELA_Y = 800; // 800 pixels na vertical
const int TELA_X = 1200; // 1200 pixels na horizontal

const int GRASS_Y = 60; // barra no fim da tela que delimita onde os aliens vão chegar

const int NAVE_H = 50;
const int NAVE_L = 100;
const int NAVE_VEL = 10;

const int ALIEN_L = 60;
const int ALIEN_H = 30;
const float ALIEN_X_VEL = 5;
const int ALIENS_INIMIGOS = 55;
const int ESPACO_X_ENTRE_ALIENS = 90;
const int ESPACO_Y_ENTRE_ALIENS = 60;
const int LINHAS = 5;
const int COLUNAS = 11;

/**/

/**************CRIANDO CENÁRIO********************/

void desenha_cenario()
{
	al_clear_to_color(al_map_rgb(0,0,0)); // limpar tela com preto
	al_draw_filled_rectangle( 0, TELA_Y - GRASS_Y, 
							 TELA_X, TELA_Y, 
							 al_map_rgb(150,150,150));

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

//faz o alien voltar no lado oposto a extremidade que ele alcançou
void restringe_nave_na_tela(Nave_t *nave)
{
	if(nave->x > TELA_X)
	{
		nave->x = 1;
	}
	if(nave->x < 0)
	{
		nave->x = TELA_X;
	}
}
/**/

/********************* CRIANDO ALIEN ***************************/

typedef struct alien {
		float x, y;
		float x_vel, y_vel;
		ALLEGRO_COLOR cor;

} Alien_t;

// INICIALIZA O ALIENS NA TELA
void inicializa_alien(Alien_t *alien)
{
	for(int i = 0; i < LINHAS; i++)
	{	for(int j = 0; j < COLUNAS; j++) // posiciona 
		{
			int elemento = i * COLUNAS + j;
			alien[elemento].x = 0 + j * ESPACO_X_ENTRE_ALIENS;
			alien[elemento].y = 25 + i * ESPACO_Y_ENTRE_ALIENS;
			alien[elemento].x_vel = ALIEN_X_VEL;
			alien[elemento].y_vel = ALIEN_H;
			alien[elemento].cor = al_map_rgb(rand()%256, rand()%256, rand()%256 );
		}
	}	
}

// DESENHO DO ALIEN
void desenha_aliens(Alien_t *alien, int tamanho)
{
	for(int i = 0; i < tamanho; i++)
	al_draw_filled_rectangle( alien[i].x , alien[i].y ,
							  alien[i].x + ALIEN_L , alien[i].y + ALIEN_H ,
							 alien[i].cor);
	
}
// 	MOVIMENTO DO ALIEN
void movimenta_alien_na_tela(Alien_t *alien)
{
	int inverte_direcao = 0;
	for(int i = 0; i < LINHAS * COLUNAS; i++)
	{	
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
				alien[i].y += alien[i].y_vel;
				alien[i].x_vel *= -1;
		}			
	}

	for(int i = 0; i < LINHAS * COLUNAS; i++)
	{
		alien[i].x += alien[i].x_vel; // movimento padrão horizontal dos aliens
	}
	
}
/**/

/********************* OS TIROS ***************************/
//NAVE
void tiro_da_nave(){}

//ALIEN
void tiro_do_alien(){}
/**/

/********************* COLISÕES ***************************/

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

int colisao_tiro_nave_alien(){}

int colisao_tiro_alien_nave(){}

/**/

int main(int argc, char **argv) {

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_EVENT ev;

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

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) 
	{
		fprintf(stderr, "falha ao carregar o modulo tff font!\n");
		return -1;
	}

	/**/


 	/********************* CRIA INICIALIZA A FILA DE EVENTOS ***************************/

	fila_eventos = al_create_event_queue();
	if(!fila_eventos) {
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

	al_start_timer(timer);
	int direita = 0;
	int esquerda = 0;
	// Inicialização da nave
	Nave_t nave;
	inicializa_nave(&nave);

	// inicialização dos aliens
	Alien_t *alien = malloc(sizeof(Alien_t)* ALIENS_INIMIGOS);
	inicializa_alien(alien);

	int jogando = 1;
	while(jogando)
	{
		al_wait_for_event(fila_eventos, &ev);

		
		if(ev.type == ALLEGRO_EVENT_TIMER)
		{
			if (esquerda) 
			{ nave.x = nave.x - nave.vel; }
			if (direita) 
			{ nave.x = nave.x + nave.vel; }

			desenha_cenario(); // inicializando cenário

			desenha_nave(nave); // inicializa a nave no cenário

			desenha_aliens(alien, ALIENS_INIMIGOS); // inicializa o alien no cenário

			movimenta_alien_na_tela(alien);

			jogando = colisao_alien_solo(alien);

			jogando = colisao_alien_com_nave(nave, alien);

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

		if(ev.type == ALLEGRO_EVENT_KEY_UP)
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

		/*****/



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
	
 
	return 0;
}
