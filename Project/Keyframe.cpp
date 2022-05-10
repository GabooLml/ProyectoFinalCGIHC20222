#include "Keyframe.h"


Keyframe::Keyframe() {}


Keyframe::Keyframe(int pasos, int variables)
{
	num_pasos = pasos;
	num_variables = variables;
}
void Keyframe::animate(bool* play)
{
	generaInterpolaciones();
	if (*play) {
		if (i_current_step >= num_pasos) //end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > frames.size() - 2)	//end of total animation?
			{
				printf("Frame index= %d\n", playIndex);
				printf("termina anim\n");
				playIndex = 0;
				*play = false;
			}
			else //Next frame interpolations
			{	
				i_current_step = 0; 
			}
		}
		else
		{
			//Draw animation
			for (int i = 0; i < valores.size(); i++) {
				printf("Valor de animacion: \n");
				valores[i] += incrementales[playIndex][i];
			}
			i_current_step++;
		}
	}
}


void Keyframe::almacenaPasos(std::vector<float> paso)
{
	if (first_frame) {
		first_frame = !first_frame;
		valores = paso;
	}

	frames.push_back(paso);
}
void Keyframe::generaInterpolaciones(void)
{
	int iteraciones = frames.size() - 2;
	std::vector<float> aux;
	for (int i = 0; i <= iteraciones; i++) {
		aux = {};
		for (int j = 0; j < frames[i].size(); j++) {
			printf("Valores actuales [%d][%d] valor: %f\n", i, j, frames[i][j]);
			printf("Generando interpolaciones [%d][%d] valor: %f\n", i, j, (frames[i + 1][j] - frames[i][j]) / num_pasos);
			aux.push_back((frames[i + 1][j] - frames[i][j]) / num_pasos);
		}
		incrementales.push_back(aux);
	}
}

void Keyframe::imprimirValores(void) {
	printf("De los frames");
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf(" %d \t", frames[i][j]);
		}
		printf("\n");
	}

}
Keyframe::~Keyframe()
{
}

