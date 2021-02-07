/*--------------------------------------------------------------------------------------------------
- Author :  Yunus KIZILTEPE
- Date   :  07.02.2021-16:55
- Version:

- File: update_android_ui_in_posix_thread.c

- Description:

- Warnings \

--------------------------------------------------------------------------------------------------*/
/*------------------------------ TAB INCLUDE -----------------------------------------------------*/
#include <jni.h>
#include <pthread.h>

/*------------------------------ TAB DEFINE ------------------------------------------------------*/

/*------------------------------- TAB TYPES ------------------------------------------------------*/
typedef struct AndroidClsList
{
    jclass activityMain;
    //java classes you want to call in posix thread are added here
}AndroidClsList;

typedef enum UI_OPERATION_TYPE
{
    UI_SET_TEXT,
    UI_CLEAR_TEXT
}UI_OPERATION_TYPE;
/*------------------------ TAB FUNCTION ABSTRACTION ----------------------------------------------*/

/*-------------------------- TAB PUBLIC VARIABLES ------------------------------------------------*/

/*-------------------------- TAB PRIVATE VARIABLES -----------------------------------------------*/
static JNIEnv   *gJniEnv = NULL;
static jobject  gJobject = NULL;
static JavaVM   *gjvm = NULL;
pthread_t my_pin_entry_thread;
AndroidClsList androidClsList;
/*--------------------------- TAB PRIVATE FUNCTONS -----------------------------------------------*/

/*--------------------------------------------------------------------------------------------------
                             posixThread
- Brief  : Function is a posix thread
- Detail : This function consist of posix thread. In the function, android UI
           updates are made.
- Parameters \
- Returns  \
--------------------------------------------------------------------------------------------------*/
static void * posixThread(void * arg)
{
    jmethodID method;
    JavaVMAttachArgs jvmArgs;
    jvmArgs.version = JNI_VERSION_1_6;
    int isAttached = 0;
    jstring sampleText = NULL;

    while (JNI_TRUE) {

        jint res = (*gjvm)->GetEnv(gjvm, (void **)&gJniEnv, JNI_VERSION_1_6); // get java environment

        if (JNI_EDETACHED == res) {//if not attached UI thread
            res = (*gjvm)->AttachCurrentThread(gjvm, &gJniEnv, 0); //Attach UI thread
            if (JNI_OK == res) {
                isAttached = 1; // attached UI thread
            } else {
                return 0;// Failed to attach
            }
        }

        if(UI_SET_TEXT == (int)(arg)) { // if operation type is set text
            // create jstring
            sampleText = (*gJniEnv)->NewStringUTF(gJniEnv, "This Text From Posix Thread");
        }
        else if(UI_CLEAR_TEXT == (int)(arg)){ // if operation type is clear text
            //empty jstring for clear UI text
            sampleText = (*gJniEnv)->NewStringUTF(gJniEnv, "");
        }

        // Get the method id of the function in MainActivity
        method = (*gJniEnv)->GetMethodID(gJniEnv, androidClsList.activityMain,
                                    "textFromJniThread", "(Ljava/lang/String;)V");

        // call method
        (*gJniEnv)->CallVoidMethod(gJniEnv, gJobject, method, sampleText);


        if (isAttached) { // if attached current thread
            (*gjvm)->DetachCurrentThread(gjvm); // Done only when attachment was done here
        }

        break;
    }

    return NULL;
}

/*--------------------------- TAB PUBLIC FUNCTONS ------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------
                                     jniUtilJByteArrayToChar
- Brief  : Function is a JNI function.
- Detail : This function is a standart JNI function. This function is automatically called
           when the pthreadandroiduilibrary in MainActivity is loaded.
           All java classes to be called in Posix thread should be assigned in this function
- Parameters \
 -- vm   : java virtual machine
- Returns  \
 -- JNI_VERSION_1_6: JNI version
--------------------------------------------------------------------------------------------------*/
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm) {
    JNIEnv *env;
    gjvm = vm;
    if((*gjvm)->GetEnv(gjvm, (void**)&env, JNI_VERSION_1_6) != JNI_OK){
        return -1;
    }

    // assign MainActivity class
    androidClsList.activityMain = (*env)->NewGlobalRef(env, (*env)->FindClass(env,
                                        "com/yunus/androiduiinposixthread/MainActivity"));

    return JNI_VERSION_1_6;
}

/*--------------------------------------------------------------------------------------------------
                                     jniUtilJByteArrayToChar
- Brief  : Function will start Posix thread when called.
- Detail : Function will start Posix thread when called.
- Parameters \
 -- env   : JNI environment
 -- thiz  : MainActivity instance
 -- opType: Operation type for UI
- Returns  \
--------------------------------------------------------------------------------------------------*/
JNIEXPORT void JNICALL
Java_com_yunus_androiduiinposixthread_MainActivity_updateAndroidUIinPosixThread(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jint opType) {
    /* if we use MainActivity instance in pthread, we should assign to global ref*/
    if(NULL == gJobject)
        gJobject = (*env)->NewGlobalRef(env, thiz);

    gJniEnv = env;// JNI environment to global value

    //start posix thread
    pthread_create(&my_pin_entry_thread, NULL, posixThread, (void *)opType);
}
