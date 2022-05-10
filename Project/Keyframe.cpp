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
			if (playIndex > frames.size() - 3)	//end of total animation?
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
			for (int i = 0; i < frames[playIndex].size(); i++) {
				valores[i] += incrementales[playIndex][i];
				printf("Valor de animacion [%d][%d]: %f\n",playIndex, i , valores[i]);
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
	int iteraciones = frames.size() - 3;
	std::vector<float> aux;
	for (int i = 0; i <= iteraciones; i++) {
		aux = {};
		for (int j = 0; j < frames[i].size(); j++) {
			aux.push_back((frames[i + 1][j] - frames[i][j]) / num_pasos);
		}
		incrementales.push_back(aux);
	}
}

void Keyframe::imprimirValores(void) {
	printf("De los frames\n");
	printf(" Par inicial: %f, %f\n", num_pasos, num_variables);
	for (int i = 0; i < frames.size() - 1; i++) {
		for (int j = 0; j < frames[i].size(); j++) {
			printf(" %f \t", frames[i][j]);
		}
		printf("\n");
	}

}

float Keyframe::getValor(int index) {
	return valores[index];

}
Keyframe::~Keyframe()
{
}

