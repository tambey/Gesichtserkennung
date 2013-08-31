Gesichtserkennung
=================

Entwicklung eins Videoüberwachungssystem mittels Algorithmen der OpenCV-Bibliothek
==================================================================================

Über
=====

Dieses Projekt wurde mittels Algortihmen der [OpenCV-Bibliothek 2.4.4](http://opencv.willowgarage.com/wiki/) auf dem Linux Ubuntu 12.04 entwickelt.


Das Programm detektiert Gesichter anhand einer Webcam und schickt den Benutzer eine E-Mail mit Hilfe der [Libcurl Bibliohtek](http://curl.haxx.se/libcurl/)


Building Optionen
=================
Um das Programm zu benutzen muss [OpenCV-Bibliothek](http://opencv.org/downloads.html) und [Libcurl-Bibliothek](http://curl.haxx.se/download.html) auf dem Linux Betriebssystem insalliert werden.
 

Haar Klassifikator Laden
========================
String face_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml";

Kamera starten
==============

VideoCapture capture(-1); 
     if( !capture.isOpened() ){ 
             cout << "\nCamera couldn't be opened!" << endl; return -1; 
      }
     capture >> frame; 

Gesichtserkennung
=================
void detectAndDisplay( Mat frame ) {
    std::vector<Rect> faces;		
    Mat frame_gray;		

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    if(!(faces.size() == 0)) {

        for( unsigned int i = 0; i < faces.size(); i++ ) {
            rectangle( frame, cvPoint(faces[i].x, faces[i].y),
                        cvPoint(faces[i].x + faces[i].width,
                        faces[i].y + faces[i].height), Scalar(255,0,0),1,8,0);
            }
   }
  imshow( window_name, frame );
 }

Speichern
=========

Mat faceROI = frame_gray( faces[i] );
stringstream g;
g << "/home/hacker/data/Fertige Projekte/facedetect_console/Photos/image" << i << ".jpg";
imwrite(g.str(), faceROI);


Projektstatus
==============
Das Programm wird nicht mehr gepflegt.
Es sollte auch mit höheren OpenCv-Bibliothek Versionen kompatible sein.

Lizenz
======
Gesichtserkennung wurde unter der Bedingungen [BSD-Lizenz](http://de.wikipedia.org/wiki/BSD-Lizenz) freigegeben.
