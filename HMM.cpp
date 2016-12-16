// HMM.cpp: define el punto de entrada de la aplicación de consola.
//

// Elisban Flores Quenaya
// MCC UCSP
// Tarea Semana tres.
// HMM

#pragma once

#include "stdafx.h"


#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <cstdlib>

typedef char T_CHAR;
typedef long T_LONG;
typedef float T_FLOAT;
typedef double PROBABILIDAD;
typedef double CONTEO;
typedef int T_INT;
typedef long long TIPOTAM;

const T_LONG BLOQUELINEA = 512;
const T_LONG TAMPALABRA = 512;
const T_LONG NATTRIBUTES = 6;
const T_LONG NATTRIBUTESVALUES = 4 + 1;
const T_LONG NCLASS = 4;
const T_LONG LIDENTIFICADOR = 32;

/**********************************************
* FUNCION PARA OBTENER EL TAMANHO DEL ARCHIVO
**********************************************/

// Para determinar la el tamaño de un archivo
TIPOTAM TamArchivo(T_CHAR *archivo)
{
	std::ifstream stream(archivo, std::ios::binary);
	if (stream.fail())
		std::cerr << "Error al abrir el archivo: " << archivo << std::endl;
	else
	{
		stream.seekg(0, stream.end);
		TIPOTAM length = (TIPOTAM)stream.tellg().seekpos();
		stream.seekg(0, stream.beg);
		stream.close();
		return length;
	}
	return -1;
}

///////////////////////////////////////////////////
// FUNCION PARA OBTENER EL NUMERO DE LINEAS DEL ARCHIVO
///////////////////////////////////////////////////

TIPOTAM NroLineasArchivo(T_CHAR *archivo)
{
	std::ifstream stream(archivo, std::ios::binary);
	if (stream.fail())
		std::cout << "Error al abrir el archivo: " << archivo << std::endl;
	else{
		TIPOTAM cont = 0;
		T_CHAR valor[BLOQUELINEA];
		while (!stream.eof()){
			stream.getline(valor, BLOQUELINEA, '\n');
			cont++;
		}
		stream.close();
		return cont-1;
	}
	return -1;
}
//////////////////////////////////////////////
// para la tarea de HMM
//////////////////////////////////////////////

//////////////////////////////////////////////
// para los Conteos
//////////////////////////////////////////////

class PalabraCont
{
public:
	CONTEO conteo;					// cuantas  veces se repite la palabra
	T_CHAR tipoPalabra[32];			// para ver si es WORDTAG
	T_CHAR tipoClasePalabra[16];	// Clase de palabra O, I-GENE
	T_CHAR palabra[64];				// palabra
	inline void imprimir(){
		std::cout << conteo << " " << tipoPalabra << " " << tipoClasePalabra << " " << palabra<<std::endl;
	}
};

class ResumenCont
{
public:
	CONTEO conteo;			// Cuantas veces se repite la secuencia
	T_INT nroNGRAM;			// nro de clases que conforma el GRAM
	T_CHAR tipoNGRAM[16];	// etiqueta del GRAM
	T_CHAR GRAM[32];		// Secuencia del GRAM
	inline void imprimir(){
		std::cout << conteo << " " << nroNGRAM << " " << tipoNGRAM << " " << GRAM << std::endl;
	}
};

PalabraCont *palabras;
TIPOTAM npalabras;
ResumenCont *resumen;
TIPOTAM nresumen;

void CargarConteo(T_CHAR *FileConteo)
{
	std::ifstream fconteo(FileConteo);// leer archivo counts	
	if (fconteo.fail())
		std::cerr << "Error al abrir el archivo: " << FileConteo << std::endl;
	else
	{
		// se carga en memoria el conteo por palabras
		T_CHAR *bloque;
		bloque = new T_CHAR[BLOQUELINEA + 1];
		T_LONG largo;
		TIPOTAM NroLineas = NroLineasArchivo(FileConteo) - 31;// 31 es el numero de elementos donde salen los NGRAMS
		npalabras = NroLineas;
		nresumen = 31;
		T_CHAR valor[BLOQUELINEA];
		palabras = new PalabraCont[NroLineas];
		resumen = new ResumenCont[nresumen];

		// Para leer las palabras
		for (T_INT i = 0; i < NroLineas; ++i){
			fconteo.getline(bloque, BLOQUELINEA, '\n');
			largo = strlen(bloque);
			if (largo > 0){
				T_INT k = 0, accion = 0;
				for (T_INT j = 0; j <= largo; ++j){
					if (bloque[j] == ' ' || bloque[j] == '\0'){
						valor[k] = '\0';
						switch (accion++){
							case 0: {palabras[i].conteo = atol(valor);break;}
							case 1: {strcpy_s(palabras[i].tipoPalabra, valor);break;}
							case 2: {strcpy_s(palabras[i].tipoClasePalabra, valor);break;}
							case 3: {strcpy_s(palabras[i].palabra, valor);break;}
							default:break;
						}
						valor[k=0] = '\0';// resetear la palabra												
					}
					else
						valor[k++] = bloque[j];
				}
			}
		}	
		//for (int l = NroLineas - 20; l < NroLineas; ++l)
		//	palabras[l].imprimir();

		//std::cout << "kjsdfksjdflksjdf"<<std::endl;

		// Para leer Resumen
		for (T_INT i = 0; i < nresumen; ++i) {
			fconteo.getline(bloque, BLOQUELINEA, '\n');
			largo = strlen(bloque);
			if (largo > 0) {
				T_INT k = 0, accion = 0;
				for (T_INT j = 0; j <= largo; ++j) {
					if (bloque[j] == ' ' || bloque[j] == '\0') {
						valor[k] = '\0';
						switch (accion++) {
							case 0: {resumen[i].conteo = atol(valor); break; }
							case 1: {
								T_CHAR tmp[2];							
								tmp[0] = valor[0]; tmp[1] = '\0';
								resumen[i].nroNGRAM = atoi(tmp);
								strcpy_s(resumen[i].tipoNGRAM, valor);
								break; 
							}
							case 2: {
								for (; j <= largo; ++j) 
									valor[k++] = bloque[j];
								valor[k] = '\0';
								strcpy_s(resumen[i].GRAM, valor); break; 
							}						
							default:break;
						}
						valor[k = 0] = '\0';// resetear la palabra												
					}
					else
						valor[k++] = bloque[j];
				}
			}
		}
		//for (T_INT l = 0; l < 31; ++l)
		//	resumen[l].imprimir();
		fconteo.close();
	}
}


CONTEO ObtenerConteoResumen(ResumenCont *&res, T_CHAR SecuenciaGRAM[])
{
	//std::cout <<"**"<< SecuenciaGRAM<<"**"<<std::endl;
	
	for (TIPOTAM i = 0;i < nresumen;i++)
	{
		//std::cout << "**" << res[i].GRAM << "**" << std::endl;
		if (strcmp(res[i].GRAM, SecuenciaGRAM) == 0)
			return res[i].conteo;
	}
	return 0.0; // casi cero, si no existe para evitar anular
}

//////////////////////////////////////////////
// para la entradas: Clase palabra
//////////////////////////////////////////////

class Palabra
{
public:
	T_CHAR valor[TAMPALABRA];	// Palabra Observada
	inline void imprimir()
	{
		std::cout << valor;
	}
	
	// Asignacion
	Palabra& operator = (const Palabra &pal) {
		if (this != &pal) {
			strcpy_s(this->valor, pal.valor);
		}
		return *this;
	}

	Palabra& operator = (const T_CHAR *pal) {
		strcpy_s(this->valor, pal);
		return *this;
	}

	// Concatenacion
	Palabra& operator + (const Palabra &pal) {		
		strcat_s(this->valor, pal.valor);		
		return *this;
	}
	
	Palabra& operator + (const T_CHAR *pal) {
		strcat_s(this->valor, pal);
		return *this;
	}


	// Asignacion concatenacion
	Palabra& operator += (const T_CHAR *pal) {
		strcat_s(this->valor, pal);
		return *this;
	}
		
	Palabra& operator += (const Palabra &pal) {
		if (this != &pal) {
			strcat_s(this->valor, pal.valor);
		}
		else{// falta implementar cuando se adiciona a la misma palabra
			/*T_CHAR tmp[TAMPALABRA];
			strcat_s(tmp, this->valor);
			strcat_s(this->valor, tmp);
			*/
		}
		return *this;
	}
	

	// Igualdad
	T_INT operator == (const Palabra &pal) {
		if (this != &pal) {
			if (strcmp(this->valor, pal.valor) != 0)
				return 0;
			return 1;
		}
		return 1;
	}

	T_INT operator == (const T_CHAR *pal) {		
		if (strcmp(this->valor, pal) != 0)
			return 0;
		return 1;		
	}
};
typedef Palabra Observacion;
typedef Palabra TAG;


T_INT nTAGsV = 4; // la cantidad de TAGs
TAG TAGsV[4];// Los TAGs O I-GENE * STOP 

Observacion *observaciones,*observacionTest;
TIPOTAM nobservaciones, nobservacionesTest;

TAG *TAGs;
TIPOTAM nTAGs;


// Para viterbi, para almacenar las frases
Observacion X[320];
TAG Y[320];// los tag que genera viterbi

void CargarObservacionTAG(T_CHAR *FileObsTAG, Observacion *&obs, TAG *&tags)
{
	std::ifstream fobsTAG(FileObsTAG);// leer archivo counts	
	if (fobsTAG.fail())
		std::cerr << "Error al abrir el archivo: " << FileObsTAG << std::endl;
	else
	{
		// se carga en memoria el conteo por palabras
		T_CHAR *bloque;
		bloque = new T_CHAR[BLOQUELINEA + 1];
		T_LONG largo;
		TIPOTAM NroLineas = NroLineasArchivo(FileObsTAG);
		nTAGs=nobservaciones = NroLineas;
		std::cout << "Nro de lineas"<<NroLineas<<std::endl;
		//getchar();
		T_CHAR valor[BLOQUELINEA];

		obs = new Observacion[NroLineas];
		tags = new TAG[NroLineas];

		// Para leer las palabras
		for (T_INT i = 0; i < NroLineas; ++i) {
			fobsTAG.getline(bloque, BLOQUELINEA, '\n');
			largo = strlen(bloque);
			if (largo > 0) {
				T_INT k = 0, accion = 0;
				for (T_INT j = 0; j <= largo; ++j) {
					if (bloque[j] == ' ' || bloque[j] == '\0') {
						valor[k] = '\0';
						switch (accion++) {
						case 0: {obs[i]=valor; break; }
						case 1: {tags[i]=valor; break; }
						default:break;
						}
						valor[k = 0] = '\0';// resetear la palabra												
					}
					else
						valor[k++] = bloque[j];
				}
			}
			else{
				obs[i]= "";
				tags[i]= "";
			}
		}
		/*for (int l = NroLineas - 20; l < NroLineas; ++l){
			obs[l].imprimir();
			tags[l].imprimir();
		}*/
		fobsTAG.close();
	}
}

void CargarObservacion(T_CHAR *FileObs, Observacion *&obs)
{
	std::ifstream fobsTAG(FileObs);// leer archivo counts	
	if (fobsTAG.fail())
		std::cerr << "Error al abrir el archivo: " << FileObs << std::endl;
	else
	{
		// se carga en memoria el conteo por palabras
		T_CHAR *bloque;
		bloque = new T_CHAR[BLOQUELINEA + 1];
		TIPOTAM NroLineas = NroLineasArchivo(FileObs);
		nobservacionesTest = NroLineas;
		std::cout << NroLineas<<std::endl;
		getchar();
		obs = new Observacion[NroLineas];	
		// Para leer las palabras
		for (T_INT i = 0; i < NroLineas; ++i) {
			fobsTAG.getline(bloque, BLOQUELINEA, '\n');
			obs[i]= bloque;		
		}			
		for (int l = NroLineas - 20; l < NroLineas; ++l) 
			obs[l].imprimir();
		fobsTAG.close();
	}
}

TIPOTAM BuscarIndiceConteoPalabra(PalabraCont *&palabras, Observacion obs, TAG tag)
{
	TIPOTAM indice = 0;
	while(indice++<npalabras)
		if (obs==palabras[indice].palabra && tag==palabras[indice].tipoClasePalabra)
			return indice;
	return -1;
}

CONTEO BuscarConteoPalabra(PalabraCont *&palabras, Observacion obs, TAG tag)
{
	TIPOTAM indice = 0;
	//std::cout <<"valor: "<< obs.valor<<"  tag:" <<tag.valor << std::endl;
	
	while (indice++<npalabras)
		if ((obs == palabras[indice].palabra)  && (tag == palabras[indice].tipoClasePalabra))
		{
			//std::cout << "cumple"<<std::endl;
			return palabras[indice].conteo;
		}
	return 0;
}


///////////////////////////////////////////////////////////
//  P A R T E    1:
///////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Renombrar las palabras menos frecuentes con _RARE_
/////////////////////////////////////////////////////////

void Tag_RARE_(T_CHAR *FileDestino)// etiquetar con _RARE a las palabra con menos de 5 repeticiones, y grabarlo en un file..
{
	std::ofstream destino(FileDestino);
	if (destino.fail())
		std::cerr << "Error al abrir el archivo: " << FileDestino << std::endl;
	else
	{
		// se supone que ya se cargaron los conteos y las observaciones con los tag respectivos
		// entonces empiezo a recorrer las palabras observadas
		//std::cout << "TAGING _RARE_" << std::endl;
		TIPOTAM ind;
		T_CHAR bloque[512];
		for (TIPOTAM i = 0; i < nobservaciones; i++){			
			if (observaciones[i].valor[0] != '\0'){// no se taggea las lineas vacias
				CONTEO suma=0;
				for (T_INT k = 0; k < nTAGsV-2; k++) {// es nTAGValues-2 poruqe solo se considera O, IGENE				{
					suma+= BuscarConteoPalabra(palabras, observaciones[i], TAGsV[k]);
				}
				if (suma < 5)						
					observaciones[i]= "_RARE_";				
			}

			strcpy_s(bloque, observaciones[i].valor);			
			strcat_s(bloque,  " ");
			strcat_s(bloque, TAGs[i].valor);
			strcat_s(bloque, "\n");	
			destino.write(bloque, strlen(bloque));
		}
		destino.close();
		/*std::cout << "termine";
		for (TIPOTAM i = 0; i < 200; i++)
			observaciones[i].imprimir();*/
	}	
}

//////////////////////////////////////////////////////////
// Para hallar la probabilidad de emision e(x|y) = count(y->x)/county
/////////////////////////////////////////////////////////
PROBABILIDAD Emision(PalabraCont *palabras, T_CHAR *obs, T_CHAR *tag, CONTEO conty){
	CONTEO cont = 0.0;
	for (TIPOTAM i = 0; i < npalabras; i++)
		if (strcmp(palabras[i].palabra, obs) == 0 && strcmp(palabras[i].tipoClasePalabra, tag) == 0)
			cont += palabras[i].conteo;
	return cont / conty;
}

//////////////////////////////////////////////////////////
// Para generar los tagger en base a una archivo de  entrada, 
// Colocar el tagger que corresponda
/////////////////////////////////////////////////////////

void SimpleTagger(T_CHAR *FileOrigen, T_CHAR *FileDestino)
{
	CONTEO nO = ObtenerConteoResumen(resumen, "O");
	CONTEO nIGENE= ObtenerConteoResumen(resumen, "I-GENE");
	std::cout << "O: "<< nO <<" I-GENE: "<< nIGENE<<std::endl;
	Palabra max_tag_rare;

	max_tag_rare= "I-GENE";
	if (Emision(palabras, "_RARE_", "O", nO) > Emision(palabras, "_RARE_", "I-GENE", nIGENE))
		max_tag_rare= "O";
		
	std::ifstream forigen(FileOrigen);
	if (forigen.fail())
		std::cerr << "Error al abrir el archivo: " << FileOrigen << std::endl;
	else
	{
		std::ofstream fdestino(FileDestino);
		PROBABILIDAD pO, pIGENE;
		T_CHAR bloque[BLOQUELINEA];
		T_CHAR linea[BLOQUELINEA];
		TIPOTAM i=0;
		while (!forigen.eof())
		{
			forigen.getline(linea, BLOQUELINEA, '\n');			
			strcpy_s(bloque, "");
			if (linea[0] != '\0')
			{
				pO = Emision(palabras, linea, "O", nO);
				pIGENE = Emision(palabras, linea, "I-GENE", nIGENE);
				strcpy_s(bloque, linea);
				if (pO + pIGENE == 0.0)// no se encontro la palabra				
				{								
					strcat_s(bloque, " ");
					strcat_s(bloque, max_tag_rare.valor);
				}
				else 
					if (pO > pIGENE)
						strcat_s(bloque, " O");
					else
						strcat_s(bloque, " I-GENE");									
			}
			strcat_s(bloque, "\n");
			fdestino.write(bloque, strlen(bloque));			
		}
		forigen.close();
		fdestino.close();
	}
}

///////////////////////////////////////////////////////////
//  P A R T E    2:
///////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Para hallar la probabilidad de emision e(yi|yi-2,yi-1) = count(yi-2,yi-1,yi)/count(yi-2,yi-1)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

PROBABILIDAD FuncionQ(T_CHAR *y_i, T_CHAR *y_2, T_CHAR *y_1)
{	
	Palabra bloque3g;
	Palabra bloque2g;

	bloque3g = y_2; bloque3g +=  " "; bloque3g += y_1; bloque3g += " "; bloque3g += y_i;
	bloque2g = y_2; bloque2g += " "; bloque2g += y_1;
		
	//std::cout <<"3g: *"<< bloque3g <<"* 2g: *"<< bloque2g <<"*"<< std::endl;

	CONTEO num = 0.0, den = 0.0;
	
	for (T_INT i=0;i < nresumen;i++){
		//std::cout << "TipoGRAM: *" << resumen[i].tipoNGRAM <<"*" << "  GRAM: *" << resumen[i].GRAM << "*" <<std::endl;		
		if (strcmp(resumen[i].tipoNGRAM, "3-GRAM") == 0)
			if (bloque3g==resumen[i].GRAM)
				num = resumen[i].conteo;
		if (strcmp(resumen[i].tipoNGRAM, "2-GRAM") == 0)
			if (bloque2g==resumen[i].GRAM)
				den = resumen[i].conteo;			
	}
	return num / den;	
}

void Estados(T_INT ind,TAG *&tag,T_INT &ntag){
	if (ind == -1 || ind == 0){
		tag[0] = "*";tag[1] = ""; ntag = 1;
	}
	else{
		tag[0]= "O";tag[1]="I-GENE";ntag = 2;
	}
}


//////////////////////////////////////////////////////////
// Renombrar las palabras menos frecuentes con _RARE_
/////////////////////////////////////////////////////////


T_INT EsNumerico(Palabra p) {

	for (T_INT i = 0; i < strlen(p.valor); i++)
		if (p.valor[i] >= 48 && p.valor[i] <= 57)
			return 1;
	return 0;
}

T_INT EsTodoMayuscula(Palabra p) {
	for (T_INT i = 0; i < strlen(p.valor); i++)
		if (p.valor[i] < 65 || p.valor[i] > 90)
			return 0;
	return 1;
}

T_INT EsUltimaMayuscula(Palabra p) {
	T_INT i;
	for (i = 0; i < strlen(p.valor) - 1; i++)
		if (p.valor[i] < 97 || p.valor[i] > 122)
			return 0;
	if (p.valor[i] >= 65 && p.valor[i] <= 90)
		return 1;

	return 0;
}

void Tag_RAREWords_(T_CHAR *FileDestino)// etiquetar con _RARE, NUMERIC, CAPITAL LASTCAPITAL a las palabra con menos de 5 repeticiones, y grabarlo en un file..
{
	std::ofstream destino(FileDestino);
	if (destino.fail())
		std::cerr << "Error al abrir el archivo: " << FileDestino << std::endl;
	else
	{
		TIPOTAM ind;
		T_CHAR bloque[512];
		for (TIPOTAM i = 0; i < nobservaciones; i++) {
			if (observaciones[i].valor[0] != '\0') {// no se taggea las lineas vacias
				CONTEO suma = 0;
				for (T_INT k = 0; k < nTAGsV - 2; k++) {// es nTAGValues-2 poruqe solo se considera O, IGENE				{
					suma += BuscarConteoPalabra(palabras, observaciones[i], TAGsV[k]);
				}
				if (suma < 5)
				{
					if (EsNumerico(observaciones[i]))
						observaciones[i] = "_NUMERIC_";
					else
						if (EsTodoMayuscula(observaciones[i]))
							observaciones[i] = "_ALLCAPITAL_";
						else
							if (EsUltimaMayuscula(observaciones[i]))
								observaciones[i] = "_LASTCAPITAL_";
							else
								observaciones[i] = "_RARE_";
				}
			}
			strcpy_s(bloque, observaciones[i].valor);
			strcat_s(bloque, " ");
			strcat_s(bloque, TAGs[i].valor);
			strcat_s(bloque, "\n");
			destino.write(bloque, strlen(bloque));
		}
		destino.close();
	}
}




//////////////////////////////////////////////////////////
// Para generar los tagger en base a una archivo de  entrada, 
// Colocar el tagger que corresponda utilizando Viterbi
/////////////////////////////////////////////////////////

T_INT IndiceBiGram(TAG tag1,TAG tag2)
{
	if (tag1 == "*" && tag2 == "*") return 0;
	if (tag1 == "*" && tag2 == "O") return 1;
	if (tag1 == "*" && tag2 == "I-GENE") return 2;
	if (tag1 == "O" && tag2 == "O") return 3;
	if (tag1 == "O" && tag2 == "I-GENE") return 4;
	if (tag1 == "O" && tag2 == "STOP") return 5;
	if (tag1 == "I-GENE" && tag2 == "O") return 6;
	if (tag1 == "I-GENE" && tag2 == "I-GENE") return 7;
	if (tag1 == "I-GENE" && tag2 == "STOP") return 8;
	return 0;
}

PROBABILIDAD pi[320*9];// 9 por los indices de los bigram

void SetPI(TIPOTAM k, TAG tag1, TAG tag2, PROBABILIDAD valor){
	pi[k*9+IndiceBiGram(tag1,tag2)] = valor;
}

PROBABILIDAD GetPI(TIPOTAM k, TAG tag1, TAG tag2){
	return pi[k * 9 + IndiceBiGram(tag1, tag2)];
}


TAG bp[320 * 9];

void SetBP(TIPOTAM k, TAG tag1, TAG tag2, TAG valor) {
	bp[k * 9 + IndiceBiGram(tag1, tag2)] = valor;
}

T_CHAR *GetBP(TIPOTAM k, TAG tag1, TAG tag2) {
	return bp[k * 9 + IndiceBiGram(tag1, tag2)].valor;
}


void ViterbiTagger(T_CHAR *FileOrigen, T_CHAR *FileDestino, T_INT tipo)
{
	std::ifstream forigen(FileOrigen);

	if (forigen.fail())
		std::cerr << "Error al abrir el archivo: " << FileOrigen << std::endl;
	else
	{
		CONTEO nTAG[2];
		nTAG[0] = ObtenerConteoResumen(resumen, "O");
		nTAG[1] = ObtenerConteoResumen(resumen, "I-GENE");
		std::cout << "O: " << nTAG[0] << " I-GENE: " << nTAG[1] << std::endl;
	
		std::ofstream fdestino(FileDestino);
		PROBABILIDAD pO, pIGENE;
		T_CHAR bloque[BLOQUELINEA];
		T_CHAR linea[BLOQUELINEA];		
		TIPOTAM nfrase = 0;
		
		T_INT gg = 0;
		while (!forigen.eof())// (gg++<300)
		{
			// Recuperar parrafo			
			strcpy_s(bloque, "");
			forigen.getline(linea, BLOQUELINEA, '\n');

			if (linea[0] != '\0') // mientras no haya un espacio se recupera el parrafo
				X[nfrase++] = linea; // se va copiando las palabras de la frase			
			else // ya estoy en un espacio, entonces si trabajo con viterbi
			{
				// Variables temporales para PI y BP
				PROBABILIDAD max_prob, act_prob;
				TAG estado;
				PROBABILIDAD e,fq;
				
				//std::cout <<"Nro de palabras: "<< nfrase<<std::endl;
				// los estados para cada paso
				TAG *si; si = new TAG[2]; T_INT nsi = 2;
				TAG *s1; s1 = new TAG[2]; T_INT ns1 = 2;
				TAG *s2; s2 = new TAG[2]; T_INT ns2 = 2;
				
				// Decode
				
				// Inicializar PI y BP
				SetPI(0, TAGsV[2], TAGsV[2], 1.0);// pi[0][*][*]=1
				SetBP(0, TAGsV[2], TAGsV[2], TAGsV[0]);//bp[0][*][*] = "O";
				
				for (TIPOTAM k = 1; k <= nfrase; k++)
				{
					//recuperar los estados que tendran S0, S1, S2
					Estados(k - 1, s1, ns1); Estados(k - 2, s2, ns2);Estados(k,  si, nsi);
									
					for (TIPOTAM v = 0; v < nsi; v++)
					{
						e = Emision(palabras, X[k - 1].valor, si[v].valor, nTAG[v]);
						if (e == 0 && tipo != 1) // la palabra no fue encontrada // entonces es necesario consideerarlo como rara
						{
							Observacion Xtemp;
							if (tipo == 3)
							{
								if (EsNumerico(X[k - 1]))
									Xtemp = "_NUMERIC_";
								else
									if (EsTodoMayuscula(X[k - 1]))
										Xtemp = "_ALLCAPITAL_";
									else
										if (EsUltimaMayuscula(X[k - 1]))
											Xtemp = "_LASTCAPITAL_";
										else
											Xtemp = "_RARE_";
							}
							else
								Xtemp = "_RARE_";
							e = Emision(palabras, Xtemp.valor, si[v].valor, nTAG[v]);
						}						

						for (TIPOTAM u = 0; u < ns1; u++)
						{
							// Inicializando los temporales
							estado = ""; max_prob = -1.0; act_prob = -1.0;

							for (TIPOTAM w = 0; w < ns2; w++) {
								fq = FuncionQ(si[v].valor, s2[w].valor, s1[u].valor);
								act_prob = GetPI(k - 1, s2[w], s1[u]) * fq * e;

								if (act_prob > max_prob) {
									estado = s2[w];	max_prob = act_prob;
								}
							}
							SetPI(k, s1[u], si[v], max_prob);
							SetBP(k, s1[u], si[v], estado);					
						}
					}
				}						
								
				Estados(nfrase, si, nsi);
				Estados(nfrase - 1, s1, ns1);
				
				PROBABILIDAD prevmax = -1.0, actual;
				TAG TYi, TY1;
				TYi = "";
				TY1 = "";
				// la probabilidad de que finalice en la palabra
				for (T_INT v = 0; v < nsi; v++) {
					for (T_INT u = 0; u < ns1; u++) {
						actual = GetPI(nfrase , s1[u], si[v]) * FuncionQ("STOP", s1[u].valor, si[v].valor);						
						if (actual > prevmax) {
							TYi = si[v];
							TY1 = s1[u];
							prevmax = actual;
						}						
					}
					//std::cout << std::endl;
				}
				
				// Trace back
				
				Y[nfrase] = TYi;
				Y[nfrase - 1] = TY1;
				
								
				for (T_INT k = nfrase - 2; k >0; k--)
					Y[k] = GetBP(k+2, Y[k+1], Y[k+2]);
				
				// se escribe en el archivo de salida								
				for (TIPOTAM l = 1; l <= nfrase; l++)	{
					strcpy_s(bloque, ""); strcat_s(bloque, X[l-1].valor);	strcat_s(bloque, " ");strcat_s(bloque, Y[l].valor);strcat_s(bloque, "\n");
					fdestino.write(bloque, strlen(bloque));
				}
				strcpy_s(bloque, ""); strcat_s(bloque, "\n");
				fdestino.write(bloque, strlen(bloque));
				nfrase = 0;
			}
		}
		forigen.close();
		fdestino.close();
	}
}



int main()
{
	// Para renombrar las palabras menos frecuentes con _RARE_
	TAGsV[0]= "O";
	TAGsV[1]="I-GENE";
	TAGsV[2] = "*";
	TAGsV[3] = "STOP";

	//SetPI(0, TAGsV[0], TAGsV[1], 0.1);
	//std::cout << GetPI(10, TAGsV[0], TAGsV[1]);
	
	// correr las partes por separado

	////////////////////////////////
	//PARTE 1: ENTRENAMIENTO
	//////////////////////////////
	/*std::cout << "Generando conteos desde python " << std::endl;
	system("python h1/count_freqs.py h1/gene.train > h1/gene.counts ");	
	getchar();

	std::cout << "Cargando Conteos desde: gene.counts" << std::endl;
	CargarConteo("h1/gene.counts");
	
	std::cout << "Cargando Observaciones y TAGs desde: gene.train" << std::endl;
	CargarObservacionTAG("h1/gene.train", observaciones, TAGs);
	
	
	std::cout << "Insertando TAG _RARE_ en: genep1.train" << std::endl;
	Tag_RARE_("h1/genep1.train");
	
	std::cout << "Generando nuevos conteos desde python en: genep1.counts  " << std::endl;
	system("python h1/count_freqs.py h1/genep1.train > h1/genep1.counts ");	
	*/
	
	////////////////////////////////////////////////
	//PARTE 1: EVALUACION CON SIMPLE TAGGER
	////////////////////////////////////////////////
	/*	
	std::cout << "Cargando Conteos desde: genep1.counts" << std::endl;
	CargarConteo("h1/genep1.counts");

	std::cout << "Cargando Observaciones y TAGs desde: genep1.train" << std::endl;
	CargarObservacionTAG("h1/genep1.train", observaciones, TAGs);
	
	std::cout << "Generando TAGs con SimpleTagger para: gene.dev" << std::endl;
	SimpleTagger("h1/gene.dev", "h1/gene_dev.p1.out");
	
	std::cout << "Evaluando resultados en Python desde gene_dev.p1.out: " << std::endl;
	system("python h1/eval_gene_tagger.py h1/gene.key h1/gene_dev.p1.out");
	
	std::cout << "Generando TAGs con SimpleTagger para: gene.dev" << std::endl;
	SimpleTagger("h1/gene.test", "h1/gene_test.p1.out");

	std::cout << "Terminado, eso creo...." << std::endl;
	*/

	////////////////////////////////
	//PARTE 2: ENTRENAMIENTO
	//////////////////////////////
	/*std::cout << "Generando conteos desde python " << std::endl;
	system("python h1/count_freqs.py h1/gene.train > h1/gene.counts ");	
	getchar();

	std::cout << "Cargando Conteos desde: gene.counts" << std::endl;
	CargarConteo("h1/gene.counts");
	
	std::cout << "Cargando Observaciones y TAGs desde: gene.train" << std::endl;
	CargarObservacionTAG("h1/gene.train", observaciones, TAGs);
	
	
	std::cout << "Insertando TAG _RARE_ en: genep2.train" << std::endl;
	Tag_RARE_("h1/genep2.train");
	
	std::cout << "Generando nuevos conteos desde python en: genep2.counts  " << std::endl;
	system("python h1/count_freqs.py h1/genep2.train > h1/genep2.counts ");	
	*/
	
	////////////////////////////////////////////////
	//PARTE 2: EVALUACION CON VITERBI TAGGER
	////////////////////////////////////////////////
	/*
	std::cout << "Cargando Conteos desde: genep1.counts" << std::endl;
	CargarConteo("h1/genep2.counts");

	std::cout << "Cargando Observaciones y TAGs desde: genep1.train" << std::endl;
	CargarObservacionTAG("h1/genep2.train", observaciones, TAGs);
	
	std::cout << "Generando TAGs con ViterbiTagger para: gene.dev" << std::endl;
	ViterbiTagger("h1/gene.dev", "h1/gene_dev.p2.out",2);
	
	std::cout << "Evaluando resultados en Python desde gene_dev.p2.out: " << std::endl;
	system("python h1/eval_gene_tagger.py h1/gene.key h1/gene_dev.p2.out");

	std::cout << "Generando TAGs con ViterbiTagger para: gene.test" << std::endl;
	ViterbiTagger("h1/gene.test", "h1/gene_test.p2.out",2);
	
	
	std::cout << "Terminado, eso creo...." << std::endl;
	*/

	////////////////////////////////
	//PARTE 3: ENTRENAMIENTO
	//////////////////////////////
	/*std::cout << "Generando conteos desde python " << std::endl;
	system("python h1/count_freqs.py h1/gene.train > h1/gene.counts ");	
	getchar();

	std::cout << "Cargando Conteos desde: gene.counts" << std::endl;
	CargarConteo("h1/gene.counts");
	
	std::cout << "Cargando Observaciones y TAGs desde: gene.train" << std::endl;
	CargarObservacionTAG("h1/gene.train", observaciones, TAGs);	
	
	std::cout << "Insertando TAG _RARE_Words en: genep3.train" << std::endl;
	Tag_RAREWords_("h1/genep3.train");
	
	std::cout << "Generando nuevos conteos desde python en: genep3.counts  " << std::endl;
	system("python h1/count_freqs.py h1/genep2.train > h1/genep3.counts ");	
	*/

	////////////////////////////////////////////////
	//PARTE 3: EVALUACION CON VITERBI TAGGER
	////////////////////////////////////////////////

	std::cout << "Cargando Conteos desde: genep1.counts" << std::endl;
	CargarConteo("h1/genep3.counts");

	std::cout << "Cargando Observaciones y TAGs desde: genep1.train" << std::endl;
	CargarObservacionTAG("h1/genep3.train", observaciones, TAGs);

	std::cout << "Generando TAGs con ViterbiTagger para: gene.dev" << std::endl;
	ViterbiTagger("h1/gene.dev", "h1/gene_dev.p3.out", 3);

	std::cout << "Evaluando resultados en Python desde gene_dev.p2.out: " << std::endl;
	system("python h1/eval_gene_tagger.py h1/gene.key h1/gene_dev.p3.out");

	std::cout << "Generando TAGs con ViterbiTagger para: gene.test" << std::endl;
	ViterbiTagger("h1/gene.test", "h1/gene_test.p3.out", 3);

	std::cout << "Terminado, eso creo...." << std::endl;
	

	getchar();
	return 0;
}