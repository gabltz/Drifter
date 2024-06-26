/*Semaine projet Drifter
  Sonar
  Mougin Léane
  24/04/24
  version 1
*/

// Fichier de d�finition de la classe Sonar
/*
Le sonar contient un capteur de lumi�re attach� sur un servo moteur allant jusqu'a 270° qui permet sa rotation
Il doit scanner tout l'environnement autour de lui et analyser si il y a une zone plus lumineuse
que le reste et nous indiquer la zone en degr�.
*/

#ifndef SONAR_H
#define SONAR_H

//Bbibliotheques et constantes necessaires au servo moteur
#include <Wire.h>                       // pour utilisation de l'I2C.
#include <Adafruit_PWMServoDriver.h>    // pour module PCA9586 16 chanel 12 bits (violet)
#define MIN_PULSE_WIDTH  600   //  Largeur minimum de l'impulsion: pour puissance minimum
#define MAX_PULSE_WIDTH 2600   //  Largeur maximum de l'impulsion : pour Puissance max
#define FREQUENCY        50    //  50hz d'origine.

#define MAX_VAL 40
#define PIN_CAPT_LUM A1
#define CANALSERVO 4
#define DELTAOFEU 50
using namespace std;


class Sonar
{
public:
	// Constructeurs
	
	void Sonar() {
	etat = 0;
	orientation = 270;
	dateDebut = millis();
	nbMesure = 10;
	intervalle = 200;
	avance = 0;
	feu = -1;
	sens = 1;
	nbTours = 0;
	}                //initialisé à 10 valeurs sur 270° */


    
	void Sonar(int angle, int nbMes, int inter,Adafruit_PWMServoDriver *pwm) {
	etat = 0;
	dateDebut = millis();
	orientation = angle;
	nbMesure = nbMes;
	intervalle = inter;
	avance = 0;
	feu = -1;
	sens = 1;
	nbTours = 0;
	pwm = pwm;
	}
	// Getters
	int getOrientation() const
	{
		return orientation;
	}

	bool getEtat() const
	{
		return etat;
	}

	int getNbMesure() const{
	return nbMesure;
	}

	int getLecture(int i) const
	{
		return lecture[i];
	}

	long getDateDebut() const
	{
		return dateDebut;
	}

	int getIntervalle() const
	{
		return intervalle;
	}

	int getAvance() const
	{
		return avance;
	}

	int getFeu() const
	{
		return feu;
	}

	int getSens() const
	{
		return sens;
	}

	int getNbTour() const
	{
	return nbTours;
	}

	//Setters
	void setOrientation(int orientation)
	{
	this->orientation = orientation;
	}

	void setEtat(bool etat)
	{ 
	this->etat = etat;
	}

	void setNbMesure(int nbM){
	this->nbMesure = nbM;
	}

	void setLecture(int rang, int val)
	{
	this->lecture[rang] = val;
	}

	void setDateDebut(long date)
	{
	this->dateDebut = date;
	}

	void setIntervalle(int intervalle)
	{
	this->intervalle = intervalle;
	}

	void setAvance(int avance)
	{
	this->avance = avance;
	}

	void setFeu(int feu)
	{
	this->feu = feu;
	}

	void setSens(int sens)
	{ 
	this->sens = sens;
	}

	void setNbTour(int tour)
	{
	this->nbTours = tour;
	}


	//Autres fonctions
	int pulseWidth(int angle) {
		int pulse_wide, analog_value;
		pulse_wide = map(angle, 0, getOrientation(), MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
		analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
		return analog_value;
	}

	void miseOn(){
		setEtat(1);
		pwm->begin(); 
		pwm->setPWMFreq(FREQUENCY);
		pwm->setPWM(CANALSERVO, 0, pulseWidth(0));                      //on met le servo à l'angle 0     
		setDateDebut(millis());                                         // on indique le temps du lancement
	}  

	void miseOff(){
		setEtat(0);
		pwm->setPWM(CANALSERVO, 0, pulseWidth(135));   // on le remet a son angle d'origine   
		setSens(1);
	}   

	void maj(){
		if(this->etat){
			//Permet de faire les mesures dans les deux sens et d'incrementer le nombre de tour
			if(getAvance() + 1 == getNbMesure() && getSens()==1){                     //+1 car on part de 0 et pas de 1
			setSens(-1);
			setNbTour(getNbTour() + 1);         
			}else if(getAvance()  == 0 && getSens()==-1){
			setSens(1);
			setNbTour(getNbTour() + 1);
			}
			if(getSens()==1){
			if(millis()>getDateDebut() + getAvance() * getIntervalle() + getNbTour() * getNbMesure() * getIntervalle()){                              //tps au lancement + tps pris pour le tour courant + tps pris par les tours passé
				pwm->setPWM(CANALSERVO, 0, pulseWidth(getAvance() * (getOrientation() / getNbMesure())));                                               //pulseWidth donne les °, combien-ième mesure * nb °dans une intervalle       
				setLecture(getAvance(),lireLum());
				setAvance(getAvance() + getSens());                                                                                                     //on augmente de 1 si sens horaire et on diminue si anti-horaire
			}
			}else{                                                                                                                                      //sens anti-horaire
			if(millis()>getDateDebut() + (getNbMesure()-getAvance()) * getIntervalle() + getNbTour() * getNbMesure() * getIntervalle()){              //tps au lancement + tps pris pour le tour courant + tps pris par les tours passé
				pwm->setPWM(CANALSERVO, 0, pulseWidth(getOrientation()-((getNbMesure()-getAvance()) * (getOrientation() / getNbMesure()))));            //pulseWidth donne les °, combien-ième mesure * nb °dans une intervalle       
				setLecture(getAvance(),lireLum());
				setAvance(getAvance() + getSens());                                                                                                     //on augmente de 1 si sens horaire et on diminue si anti-horaire
			}
			}
		}
	} 

	int lireLum(){
		return analogRead(PIN_CAPT_LUM);
	}          

	int moyLum(){
		int moy = 0;
		if(getNbTour()==0){                                                   //si le tableau n'a pas encore été complettement remplit
			for(int i = 0; i<getAvance() + 1;i++){
			moy += getLecture(i);
			}
			return moy/getAvance();
		}else{                                                                //sinon on fait la moyenne de tout le tableau
			for(int i = 0; i<getNbMesure() ;i++){
			moy += getLecture(i);
			}
		}
		return moy/getNbMesure();
	} 

	int maxLum(){
		int max = getLecture(0);
		if(getNbTour()==0){                                                   //si le tableau n'a pas encore étécomplettement remplit
			for(int i = 1; i<getAvance() + 1;i++){
			if(getLecture(i)>max){
				max = getLecture(i);
			}
			}
		}else{
			for(int i = 0; i<getNbMesure() ;i++){
			if(getLecture(i)>max){
				max = getLecture(i);
			}
			}
		}
		return max;
	} 

	int rangLumMax(){
		for(int i = 0; i<getNbMesure() ;i++){
			if(getLecture(i) == maxLum()){
			return i;
			}
		}
		return -1;  //si on arrive là il y a eu un problème
	}


	int oFeu(){
		if(maxLum()-moyLum() > DELTAOFEU){
			setFeu(rangLumMax() * getOrientation() / getNbMesure());        //getOrientation / getNbMesure correspond à l'intervalle en degres entre deux mesures
		}
		return getFeu();
	}    //fonction convertisantles degrés en valeur analogique pour le servo


private:
	bool etat;			          // 1 = en marche, 0 sinon
	int orientation;	      //en degr?s
  int nbMesure;           //nombre de mesures effectuée sur une rotation
	int lecture[MAX_VAL];	  //tableau contenant les valeurs lues
	long dateDebut;	        //en ms
	int intervalle;		      //en ms
	int avance;			        //combien de mesures ont deja �t� faites
  int feu;                //indique l'angle du feu, -1 si il n'y a pas le feu
  int sens;               //horaire 1 ou anti-horaire -1
  int nbTours;            //Nombre de tours complets qu'a fait le sonar 
  Adafruit_PWMServoDriver &pwm; //variable nécésaire au servo moteur
};
#endif // !SONAR_H
