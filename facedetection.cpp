/**
 * @file objectDetection.cpp
 * @author T. Beylik ( based in the classic facedetect.cpp in samples/c )
 * @brief A simplified version of facedetect.cpp, show how to load a cascade classifier and how to find objects (Face + eyes + mouth) in a video stream
 */

/* bind the OpenCV-libraries to use them */
#include "opencv2/objdetect/objdetect.hpp"		/* For detect poeples and faces*/
#include "opencv2/highgui/highgui.hpp"			/* Start, Save Video Stream */
#include "opencv2/imgproc/imgproc.hpp"			/* Image Processing */

/* bind the libcurl to SendMail */
#include <curl/curl.h>


using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame );
int Send_Email();

/* load the *.xml classifier file */
String face_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml";
String eyeglasses_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_eye_tree_eyeglasses.xml";
String mouth_cascade_name = "/usr/local/share/OpenCV/haarcascades/haarcascade_mcs_mouth.xml";

/* initialize an object of CascadeClassifier class */
CascadeClassifier face_cascade;
CascadeClassifier eyeglasses_cascade;
CascadeClassifier mouth_cascade;

/* Name of GUI */
string window_name = "Video Surveillance";

/* The Random Number generator class (RNG) and how to get a random number from a uniform distribution */
RNG rng(12345);		/* Line color of rectangle*/

/*
 * This is the list of basic details you need to tweak to get things right.
*/
#define USERNAME "tamerbeylik@live.de"
#define PASSWORD "cicekcikiz01"
#define SMTPSERVER "smtp.live.com"
#define SMTPPORT ":25" /* it is a colon+port string, but you can set it
                           to "" to use the default port */
#define RECIPIENT "<tamerbeylik@live.de>"
#define MAILFROM "<tambey-01@windowslive.com>"

#define MULTI_PERFORM_HANG_TIMEOUT 60 * 1000

/* Note that you should include the actual meta data headers here as well if
   you want the mail to have a Subject, another From:, show a To: or whatever
   you think your mail should feature! */
static const char *text[]={
	"Subject: Video Surveillance\n", /* Mail Subject */
	"\n", /* empty line to divide headers from body */ 
 	"There's someone at home!\n",
	"\n",
  NULL
};

struct WriteThis {
	int counter;	/* Zähler */
};

/*
**The read_callback() is a function which CURL calls when it need to obtain data that will be uploaded to the server.
*/
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp) {

	/* This was already in. */
    struct WriteThis *pooh = (struct WriteThis *)userp;
    const char *data;

    if(size*nmemb < 1)
        return 0;

    data = text[pooh->counter];

    if(data) {
        size_t len = strlen(data);
        memcpy(ptr, data, len);
        pooh->counter++; /* advance pointer */
		return len;  /* return the size we would have saved */
    }
    return 0;                         /* no more data left to deliver */
}

static struct timeval tvnow(void) {
    /*
    ** time() returns the value of time in seconds since the Epoch.
    */

	/* A struct timeval represents a time as a number of seconds (tv_sec) plus a number of microseconds (tv_usec) between 0 and 1,000,000. */
    struct timeval now;
    now.tv_sec = (long)time(NULL);
    now.tv_usec = 0;
    return now;
}

static long tvdiff(struct timeval newer, struct timeval older) {
    return (newer.tv_sec-older.tv_sec)*1000+
           (newer.tv_usec-older.tv_usec)/1000;
}


/** @function main */
/** start the camera and hand over the variable to the function detectAndDisplay*/
int main( int argc, const char** argv ) {
	 CvCapture* capture; /* a parameter for video capturing functions */
	 Mat frame;			 /* 2D or multi-dimensional dense array for frame*/

     /* 1. Load the cascades */
     if( !eyeglasses_cascade.load( eyeglasses_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
     if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
     if( !mouth_cascade.load( mouth_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

     /* 2. Read the video stream */
     capture = cvCaptureFromCAM( -1 );
     if( capture )
     {
         while( true )
         {
			 /* using cvQueryFrame to get each frame */
             frame = cvQueryFrame( capture ); 

             /* 3. Apply the classifier to the frame */
             if( !frame.empty() )
             {
				 /* call the function to detect faces */
                 detectAndDisplay( frame );
             }
             else
             {
                 printf(" --(!) No captured frame -- Break!"); break;
             }

			 /* Press Esc to close face detection */
			 char c = cvWaitKey(33);  /* The 33 number in the code means that after 33ms, a new frame would be shown. */
			 if (c == 27) { break; } /* 27 is the Ascii Code fpr Esc*/
          }
	 }
     return 0;
 }

/** @function detectAndDisplay */
/** this function detect face draw a rectangle around and detect eye & mouth and draw a circle around */
void detectAndDisplay( Mat frame ) {
    std::vector<Rect> faces;		 /* For calculate center's X,Y coordinates of Objects */
	Mat frame_gray;		/* Matrix of an Frame */

	/* Convert it to gray */
    cvtColor( frame, frame_gray, CV_BGR2GRAY );
	/* Equalizes the histogram of a grayscale image */
    equalizeHist( frame_gray, frame_gray );

	/* Detect faces (detectMultiScale to perform the detection) */
	/* The flag CV_HAAR_SCALE_IMAGE, tells the algorithm to scale the image rather than the detector. */
    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );


    if(!(faces.size() == 0)) {

        for( unsigned int i = 0; i < faces.size(); i++ ) {
            rectangle( frame, cvPoint(faces[i].x, faces[i].y),
                        cvPoint(faces[i].x + faces[i].width,
                        faces[i].y + faces[i].height), Scalar(255,0,0),1,8,0);

            /* Save detected face */
            Mat faceROI = frame_gray( faces[i] );	/* Region of interest */
            stringstream g;
            g << "/home/hacker/data/Fertige Projekte/facedetect_console/Photos/image" << i << ".jpg";
            imwrite(g.str(), faceROI);

            std::vector<Rect> eyes;

            /* In each face, detect eyes */
            eyeglasses_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

            for( unsigned int j = 0; j < eyes.size(); j++ ) {
                Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5,
                              faces[i].y + eyes[j].y + eyes[j].height*0.5 );

                int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
                circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }

            std::vector<Rect> mouth;

            /* In each face, detect eyes */
            mouth_cascade.detectMultiScale(faceROI, mouth, 1.1, 2,2|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

            for( unsigned int k = 0; k < mouth.size(); k++ ) {
                Point center( faces[i].x + mouth[k].x + mouth[k].width*0.5,
                              faces[i].y + mouth[k].y + mouth[k].height*0.5 );

                int radius = cvRound( (mouth[k].width + mouth[k].height)*0.25 );
                circle( frame, center, radius, Scalar( 255, 0, 0 ), 4, 8, 0 );
            }
        }
        //-- Send Email if any face detected
        // Send_Email();
    }
  /* Show what you got */
  imshow( window_name, frame );
 }

 int Send_Email() {
	 CURL *curl;				/* curl */
	 CURLM *mcurl;				/* multiple curl */
	 int still_running = 1;		/* keep number of running handles */
     struct timeval mp_start;
     struct WriteThis pooh;
	 struct curl_slist* rcpt_list = NULL; /* rcpt = recipients*/

     pooh.counter = 0;

	 /* In windows, this will init the winsock stuff */
     curl_global_init(CURL_GLOBAL_DEFAULT);

	 /* get a curl handle */
     curl = curl_easy_init();
     if(!curl)
       return 1;

	 /* create a multi handle */
     mcurl = curl_multi_init();

     if(!mcurl)
       return 2;

	 /* CURLOPT_MAIL_RCPT takes a list, not a char array.  */
     rcpt_list = curl_slist_append(rcpt_list, RECIPIENT);
     /* more addresses can be added here
        rcpt_list = curl_slist_append(rcpt_list, "<others@example.com>");  */

	 /* First set the URL that is about to receive our POST. */
	 curl_easy_setopt(curl, CURLOPT_URL, "smtp://" SMTPSERVER SMTPPORT);

	 /* set user name for the authentication */
     curl_easy_setopt(curl, CURLOPT_USERNAME, USERNAME);

	 /* set password for the authentication */
	 curl_easy_setopt(curl, CURLOPT_PASSWORD, PASSWORD);

	 /* send all data to this function  */
	 curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

	 /* It will be used to specify the sender  address in a mail when sending an SMTP mail with libcurl. */
     curl_easy_setopt(curl, CURLOPT_MAIL_FROM, MAILFROM);

	 /* Pass a pointer to a linked list of recipients to pass to the server in your SMTP mail request. */
     curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, rcpt_list);

	 /*  Require SSL for all communication or fail with CURLE_USE_SSL_FAILED. */
     curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

	 /* This  option  determines  whether  curl verifies the authenticity of the peer's certificate. A
	 value of 1 means curl verifies; 0 (zero) means it doesn't */
     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

	 /* Use CURLOPT_SSL_VERIFYHOST to control that. The check that the host name in the
	 certificate is valid for the host name you're connecting to is done independently of the  CUR-
	 LOPT_SSL_VERIFYPEER option. */
     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

	 /* pointer to pass to our read function */
     curl_easy_setopt(curl, CURLOPT_READDATA, &pooh);

	 /* get verbose debug output please */
     curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

     curl_easy_setopt(curl, CURLOPT_SSLVERSION, 0L);

	 /*  Pass  a  long  set  to  0 to disable libcurl's use of SSL session-ID caching. Set this to 1 to enable it.
	 By default all transfers are done using the cache.  */
     curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);

	 /* add an easy handle to a multi session */
     curl_multi_add_handle(mcurl, curl);

     mp_start = tvnow();

     /* we start some action by calling perform right away */
     curl_multi_perform(mcurl, &still_running);

     while(still_running) {
         struct timeval timeout;

         int rc; /* select() return code */

         fd_set fdread;
         fd_set fdwrite;
         fd_set fdexcep;
         int maxfd = -1;

         long curl_timeo = -1;

         FD_ZERO(&fdread);
         FD_ZERO(&fdwrite);
         FD_ZERO(&fdexcep);

         /* set a suitable timeout to play around with */
		 timeout.tv_sec = 1;		/* 1000 = 1 second*/
		 timeout.tv_usec = 0;		/* u second = u is the SI prefix for micro */

		/* if the curl timeout function returns something longer than 0, then don't wait any longer than that. */
         curl_multi_timeout(mcurl, &curl_timeo);
         if(curl_timeo >= 0) {
			 timeout.tv_sec = curl_timeo / 1000;		
             if(timeout.tv_sec > 1)
				 timeout.tv_sec = 1;					/* set time out 1 second */
             else
				 timeout.tv_usec = (curl_timeo % 1000) * 1000;	/* converte to milliseconds for tv_usec */
         }

      /* get file descriptors from the transfers */
      curl_multi_fdset(mcurl, &fdread, &fdwrite, &fdexcep, &maxfd);

      /* In a real-world program you OF COURSE check the return code of the
        function calls.  On success, the value of maxfd is guaranteed to be
        greater or equal than -1.  We call select(maxfd + 1, ...), specially in
        case of (maxfd == -1), we call select(0, ...), which is basically equal
        to sleep. */

      rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);

	  /* call tvdiff funcition */
      if (tvdiff(tvnow(), mp_start) > MULTI_PERFORM_HANG_TIMEOUT) {
          fprintf(stderr, "ABORTING TEST, since it seems "
               "that it would have run forever.\n");
       break;
      }

      switch(rc) {
         case -1:
         /* select error */
         break;
         case 0: /* timeout */
         default: /* action */
			 curl_multi_perform(mcurl, &still_running);		    /* timeout or readable/writable sockets */
         break;
      }
    }
	 /* free the rcpt(recipients) list*/
    curl_slist_free_all(rcpt_list);
    curl_multi_remove_handle(mcurl, curl);

	/* always cleanup */
	curl_multi_cleanup(mcurl);
    curl_easy_cleanup(curl);

	/* we're done with libcurl, so clean it up */
	curl_global_cleanup();
    return 0;
}
