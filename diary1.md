# Dagbok #
## Vår Period 1 ##

Under julen lärde vi oss basics i C++. Under hela period 1 hade vi labbar, så när vi fick tid över satt vi och gick igenom alla tutorials.

Viktiga milstolpar:

  * Får Ogre att kompilera
  * Egen chasingcam
  * Importera modeller från 3D studio max
  * Importera scener från 3D studio max
  * Använda fysikbiblioteket Bullet för collisiondetection

---

## 17 mars ##

Spelidé

  * Målet med spel (vad går spelet ut på) : Att komma till slutet av banan så fort som möjligt

  * På vägen dit möter man hinder vilket man behöver olika spells/abilities för att kunna ta sig igenom. Dessa spells samlar man på sig utmed banan och aktiverar genom olika knapptryckskombinationer


Heldag med projektet, tittat på en tutorial vid namn practical application som kan hantera states, gui etc. Vi har även fått igång ankhsvn tillsammans med google code och det fungerar bra!

Nästa steg blir;

  * Steg 1: hantera gamestates
  * Steg 2: Specifiera klasser
  * Steg 3: implementera klasser

---

## 18 mars ##

![http://projectkarma.googlecode.com/files/gamestateplusbullet.jpg](http://projectkarma.googlecode.com/files/gamestateplusbullet.jpg)
Fått Bullet Physics(http://www.bulletphysics.org/) och dot scene Loader att fungera med advanced ogre framework! Woho!

---

## 27 mars ##

MTD och SoS dugga över -> mer tid till 3D programmering! Börjat titta på vilket ljudbibliotek vi ska använda. OpenAL verkar användbart men får inte skiten att kompilera(alla tutorials och guider använder en gammal och ouppdaterad openAL).

---

## 28 mars ##

Ljudbiblioteket irrKlang(http://www.ambiera.com/irrklang/) kommer att användas i detta projektet. Riktigt smidigt att hantera och skapa 3D ljud med!

Idag har klassen SoundManager skrivits. Det är tänkt att en instans till den ska finnas med i rooten och på så sätt kan SoundManagern nås överallt genom Ogre::Root->getSingelton->m\_Sound

---

## 29 mars ##

Heldag hemma hos Per idag. Har för det mesta tittat igenom exportering av animering och texturer från 3D studio max till Ogre. Vi kommer att använda oss av OgreMax(http://www.ogremax.com/) som tillåter 3DS Max->Ogres .mesh eller .scene(för dot scene loader). Finns även möjlighet att exportera animation. I hjälpfilerna till OgreMax fanns som tur var exempel på hur man använde programmet.

---

## 30 mars ##

Ännu en heldag hos Per. Ägnade hela morgonen till att konstantera att oändligt(nästan) många timmar har lagts på Bullet Physics, helt i onödan. I Bullet kan vi nu dels Raycasta en stråle ner från gubbens midja för att sedan kollidera med närmsta punkt och dels använda en cylinder som kolliderar med omvärlden.
Collision Detection funktionerna har vi fått skriva själva och de fungerar, dock "fastnar" gubben när den går in i en vägg. Vi vill att den ska "slajda", något man behöver använda fysik för. I Bullet finns en kinematicCharacterController till detta, men den är dels svårimplementerad och dels så finns det mkt i @todo i sourcefilen.

Vi beslutade därför att byta fysikmotor.
Ladies and gentlemen.....
![http://www.shinvision.com/wp-content/uploads/2009/05/nvidia_logo2.jpg](http://www.shinvision.com/wp-content/uploads/2009/05/nvidia_logo2.jpg)

Finns jättemycket dokumentation om PhysX dock så skriver de direkt i OpenGL vilket gör det svårare för oss. Som tur är finns det en wrapper som heter NxOgre(http://nxogre.org/).

---

## 31 mars ##

Idag har ENORMA framsteg gjorts. Det började illa på morgonen, inget ville kompilera och fungera. Men på kvällen lossnade allt och sak på sak blev implementerad i spelet. Gubben "slajdar" nu, ty vi använder en osynlig kapsel (utan rotation) som vi har attachat gubben på. Sedan ger vi linjär hastighet i varje frame till kapseln för att styra. Eftersom kapseln är av typen Dynamic Rigidbody så sker Collisionhantering automatiskt i PhysX, sjukt bra!

Finns ett bra sidoprogram som heter Flour. Den gör PhysX skelett till en .mesh fil.

Lade även in massa andra rigidbodys som man kan kollidera med.
Fått motion blur(tyvärr FPS dependent) att fungera

---
