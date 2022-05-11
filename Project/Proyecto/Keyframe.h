#pragma once

#include <glew.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <glfw3.h>
#include <vector>


class Keyframe
{
public:
	Keyframe();
	Keyframe(int pasos, int variables);
	void almacenaPasos(std::vector<float> paso);
	void animate(bool* play);
	void generaInterpolaciones(void);
	void imprimirValores(void);
	float getValor(int);
	~Keyframe();

private:
	float num_variables;
	float num_pasos;
	int playIndex = 0;
	//Valor del primer frame, representa el valor actual del modelo asociado a la animación.
	std::vector<float> valores;
	//Simulará un arreglo de 2 dimensiones, frames indica cada paso y su contenido es otro vector, con las variables ej <0.0f, 1.1 >
	std::vector<std::vector<float>> frames;
	//Almacena los valores incrementales entre frames.
	std::vector<std::vector<float>> incrementales;
	int i_current_step = 0;
	bool first_frame = true;
	bool end_animation = false;
};