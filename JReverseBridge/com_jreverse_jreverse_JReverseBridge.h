/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_jreverse_jreverse_Bridge_JReverseBridge */

#ifndef _Included_com_jreverse_jreverse_Bridge_JReverseBridge
#define _Included_com_jreverse_jreverse_Bridge_JReverseBridge
#ifdef __cplusplus
extern "C" {
#endif
	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    testMethod
	 * Signature: ()Z
	 */
	JNIEXPORT jboolean JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_testMethod
	(JNIEnv*, jclass);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    InitBridge
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_InitBridge
	(JNIEnv*, jclass);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    StartAndInjectDLL
	 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
	 */
	JNIEXPORT jint JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_StartAndInjectDLL
	(JNIEnv*, jclass, jstring, jstring);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    InjectDLL
	 * Signature: (ILjava/lang/String;)I
	 */
	JNIEXPORT jint JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_InjectDLL
	(JNIEnv*, jclass, jint, jstring);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    WriteStartupPipe
	 * Signature: ([Lcom/jreverse/jreverse/StartupRule;)I
	 */
	JNIEXPORT jint JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_WriteStartupPipe
	(JNIEnv*, jclass, jobjectArray);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    SetupPipe
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_SetupPipe
	(JNIEnv*, jclass);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    PrimeLoadedClasses
	 * Signature: ()V
	 */
	JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_PrimeLoadedClasses
	(JNIEnv*, jclass);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    GetStringPipe
	 * Signature: ()Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_GetStringPipe
	(JNIEnv*, jclass);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    WriteStringPipe
	 * Signature: (Ljava/lang/String;)V
	 */
	JNIEXPORT void JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_WriteStringPipe
	(JNIEnv*, jclass, jstring);

	/*
	 * Class:     com_jreverse_jreverse_Bridge_JReverseBridge
	 * Method:    CallCoreFunction
	 * Signature: (Ljava/lang/String;[Ljava/lang/String;)[Ljava/lang/String;
	 */
	JNIEXPORT jobjectArray JNICALL Java_com_jreverse_jreverse_Bridge_JReverseBridge_CallCoreFunction
	(JNIEnv*, jclass, jstring, jobjectArray);
#ifdef __cplusplus
}
#endif
#endif
