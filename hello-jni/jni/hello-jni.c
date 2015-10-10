/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <pthread.h>

//call library
#include "prebuilt/include/libsum.h"

static const char *TAG="hello-jni";
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO,  TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)

/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
//jstring
//Java_com_example_hellojni_HelloJni_stringFromJNI( JNIEnv* env,
//                                                  jobject thiz )
//{
//#if defined(__arm__)
//  #if defined(__ARM_ARCH_7A__)
//    #if defined(__ARM_NEON__)
//      #if defined(__ARM_PCS_VFP)
//        #define ABI "armeabi-v7a/NEON (hard-float)"
//      #else
//        #define ABI "armeabi-v7a/NEON"
//      #endif
//    #else
//      #if defined(__ARM_PCS_VFP)
//        #define ABI "armeabi-v7a (hard-float)"
//      #else
//        #define ABI "armeabi-v7a"
//      #endif
//    #endif
//  #else
//   #define ABI "armeabi"
//  #endif
//#elif defined(__i386__)
//   #define ABI "x86"
//#elif defined(__x86_64__)
//   #define ABI "x86_64"
//#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
//   #define ABI "mips64"
//#elif defined(__mips__)
//   #define ABI "mips"
//#elif defined(__aarch64__)
//   #define ABI "arm64-v8a"
//#else
//   #define ABI "unknown"
//#endif
//    return (*env)->NewStringUTF(env, "Hello from JNI !  Compiled with ABI " ABI ".");
//}


jstring
stringFromJNI_native( JNIEnv* env, jobject thiz )
{
	return (*env)->NewStringUTF(env, "Hello from JNI dynamic registration!");
}

jint
PrimitiveTypes_native(JNIEnv *env, jobject thiz, jboolean a, jbyte b, jchar c, jshort d, jint e, jlong f, jfloat h, jdouble i)
{
	long _f;
	float _h;
	double _i;

	_f = f;
	_h = h;
	_i = i;

	LOGD("JNI: call PrimitiveTypes_native");
	LOGD("JNI: jboolean a=%d", a);
	LOGD("JNI: jbyte b=%d", b);
	LOGD("JNI: jchar c=%d", c);
	LOGD("JNI: jshort d=%d", d);
	LOGD("JNI: jint d=%d", e);
	LOGD("JNI: jlong d=%ld", _f);
	LOGD("JNI: jfloat h=%f", _h);
	LOGD("JNI: jfloat h=%f", _i);

	return 0;
}

#define TEST_BUFFER_SIZE 10

void
ReferenceTypesSetByteArray1_native(JNIEnv *env, jobject thiz, jbyteArray buffer)
{
	int len;
	char array[TEST_BUFFER_SIZE];

	len = (*env)->GetArrayLength(env, buffer);

	//ֱ�ӽ�Java�˵����鿽�������ص������У�����ʹ�����ַ�ʽ�����Ӱ�ȫ
	(*env)->GetByteArrayRegion(env, buffer, 0, len, array);

	//����ͨ��array��������������ֵ��,ע�⣬�޸ĵ�ֻ�Ǳ��ص�ֵ��Java�˲���ͬʱ�ı������ֵ
	int i=0;
	for( i=0; i<len; i++ ) {
		array[i] = i;
		LOGD("JNI: array[%d]=%d", i, array[i]);
	}
}

void
ReferenceTypesSetByteArray2_native(JNIEnv *env, jobject thiz, jbyteArray buffer)
{
	int len;
	int i=0;
	char array[TEST_BUFFER_SIZE];
	len = (*env)->GetArrayLength(env, buffer);

	//������ָ��ָ����Java��������ڴ��ַ������Jvm�ľ���ʵ�֣���������סJava�˵��Ƕ����鲻�����գ��������ü�������
	//Ҳ������Jvm�ڶ��϶Ը������һ�ݿ���
	//�ٶȺ�Ч�ʱ�GetByteArrayRegion����Ҫ�ߺܶ�
	char * pBuffer = (*env)->GetByteArrayElements(env,buffer,NULL);
	if( pBuffer == NULL ) {
		LOGE("GetIntArrayElements Failed!");
		return;
	}

	for (i=0; i < len; i++) {
		array[i] = pBuffer[i];
		LOGD("JNI: pBuffer[%d]=%d", i, pBuffer[i]);
	}

	//����ͨ��pBufferָ����������������ֵ��,ע�⣬�޸ĵ��Ƕ��ϵ�ֵ��Java�˿��ܻ�ͬ���ı䣬������Jvm�ľ���ʵ��,������ͨ���������ı�Java�˵�����ֵ
	for( i=0; i<len; i++ ) {
		pBuffer[i] = i + 10;
	}

	//���Ҫ�����ͷ�ָ�루��С���ü�����
	(*env)->ReleaseByteArrayElements(env,buffer,pBuffer,0);
}

void
ReferenceTypesSetDirectIntArray_native(JNIEnv *env, jobject thiz, jbyteArray buffer, int len)
{
	//���追����ֱ�ӻ�ȡ��Java�˹����ֱ���ڴ��ַ(Ч�ʱȽϸߣ���object�Ĺ������������󣬽��鳤��ʹ�õĴ���buffer�������ַ�ʽ)
	unsigned char * pBuffer = (unsigned char *)(*env)->GetDirectBufferAddress(env,buffer);
	if( pBuffer == NULL ) {
		LOGE("GetDirectBufferAddress Failed!");
		return;
	}

	//����ͨ��pBufferָ����������������ֵ��,ע�⣬�޸������ֵ��Java��ͬʱ�仯
	int i=0;
	for( i=0; i<len; i++ ) {
		pBuffer[i] = i;
	}
}

jbyteArray
ReferenceTypesGetByteArray_native(JNIEnv *env, jobject thiz)
{
	//����JNI����������ݵ�Java�ˣ������ַ�����һ���Ǳ�����ʾ��ͨ������ֵ������
	//��һ����ͨ���ص�Java�˵ĺ���������(������jni�߳��лص�java��)
	unsigned char buffer[TEST_BUFFER_SIZE];

	int i=0;
	for( i=0; i<TEST_BUFFER_SIZE; i++ ) {
		buffer[i] = i;
	}

	//����ByteArray
	jbyteArray array = (*env)->NewByteArray(env,TEST_BUFFER_SIZE);

	//���������ݿ�����java��
	(*env)->SetByteArrayRegion(env, array, 0, TEST_BUFFER_SIZE, buffer);

	return array;
}

void
OperateJavaAttribute_native(JNIEnv *env, jobject thiz)
{
	 //���jfieldID �Լ� ���ֶεĳ�ʼֵ
	jfieldID  nameFieldId ;
	jclass cls = (*env)->GetObjectClass(env, thiz);  //���Java��ö���ʵ���������ã���HelloJNI������
	nameFieldId = (*env)->GetFieldID(env, cls , "name" , "Ljava/lang/String;"); //������Ծ��
	if(nameFieldId == NULL)
	{
		LOGE("can not find java field id");
	}

	jstring javaNameStr = (jstring)(*env)->GetObjectField(env, thiz ,nameFieldId);  // ��ø����Ե�ֵ
	const char * c_javaName = (*env)->GetStringUTFChars(env, javaNameStr , NULL);  //ת��Ϊ char *����
	LOGD("JNI: java name is: %s", c_javaName);
	(*env)->ReleaseStringUTFChars(env, javaNameStr , c_javaName);  //�ͷžֲ�����

	//����һ��jString����
	char * c_ptr_name = "Native layer";
	jstring cName = (*env)->NewStringUTF(env, c_ptr_name); //����һ��jstring����
	(*env)->SetObjectField(env, thiz , nameFieldId , cName); // ���ø��ֶε�ֵ

}

void
NativeCallbackJava_native(JNIEnv *env, jobject thiz)
{
	 //�ص�Java�еķ���
	jclass cls = (*env)->GetObjectClass(env, thiz);//���Java��ʵ��
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "Java_method" , "(Ljava/lang/String;)V") ;//��øûص��������

	if(callbackID == NULL)
	{
		LOGE("getMethodId is failed \n");
	}

	jstring native_desc = (*env)->NewStringUTF(env, "callback java I am form Native");
	(*env)->CallVoidMethod(env, thiz , callbackID , native_desc); //�ص��÷��������Ҵ��ݲ���ֵ
}

jobject
NativeGetEmployeeInfo_native(JNIEnv *env, jobject thiz)
{
	LOGD("JNI invoke NativeGetEmployeeInfo_native");

	jclass employeecls = (*env)->FindClass(env, "com/example/hellojni/Employee");

	//��õø����͵Ĺ��캯��  ������Ϊ <init> �������ͱ���Ϊ void �� V
	//ע�⣺�� ID��ָ����class�Ĺ��캯��ID �� ����ͨ������ GetMethodID() ��ã��ҵ���ʱ�ķ���������Ϊ <init>�����������ͱ���Ϊ void (V)
	jmethodID constrocMID = (*env)->GetMethodID(env, employeecls, "<init>", "(ILjava/lang/String;)V");

	jstring str = (*env)->NewStringUTF(env, "xiaoming");

	jobject employeeobj = (*env)->NewObject(env, employeecls,constrocMID, 30, str);  //����һ�����󣬵��ø���Ĺ��캯�������Ҵ��ݲ���

	return employeeobj;
}

void
TransferEmployeeInfo_native(JNIEnv *env, jobject thiz, jobject employeeobj)
{
	LOGD("JNI invoke TransferEmployeeInfo_native");

    jclass employeecls = (*env)->GetObjectClass(env, employeeobj); //���Student������

    if(employeecls == NULL)
    {
        LOGE("JNI GetObjectClass failed");
    }

    jfieldID ageFieldID = (*env)->GetFieldID(env, employeecls,"age","I"); //��õ�Student�������id
    jfieldID nameFieldID = (*env)->GetFieldID(env, employeecls,"name","Ljava/lang/String;"); // �������ID

    jint age = (*env)->GetIntField(env, employeeobj , ageFieldID);  //�������ֵ
    jstring name = (jstring)(*env)->GetObjectField(env, employeeobj , nameFieldID);//�������ֵ

    const char * c_name = (*env)->GetStringUTFChars(env, name ,NULL);//ת���� char *

    LOGD("JNI name:%s, age:%d", c_name, age);
    (*env)->ReleaseStringUTFChars(env, name,c_name); //�ͷ�����
    //LOGD("JNI name:%s, age:%d", c_name, age); �������ͷź��ڵ���
}

JavaVM * gJavaVM;
jobject  gJavaObj;

static void* native_thread_exec(void *arg) {

    JNIEnv *env;
    LOGD("JNI: pthread tid=%lu, pid=%lu", (unsigned long)gettid(), (unsigned long)getpid());
    //��ȫ�ֵ�JavaVM�л�ȡ����������
    (*gJavaVM)->AttachCurrentThread(gJavaVM,&env, NULL);

 //   //��ȡJava���Ӧ����
    jclass cls = (*env)->GetObjectClass(env,gJavaObj);
    if( cls == NULL ) {
    	LOGE("JNI Fail to find javaClass");
    	return 0;
    }

//    //��ȡJava�㱻�ص��ĺ���
    jmethodID callbackID = (*env)->GetMethodID(env, cls, "Java_method" , "(Ljava/lang/String;)V") ;//��øûص��������
	if(callbackID == NULL)
	{
		LOGE("getMethodId is failed \n");
	}

	jstring str = (*env)->NewStringUTF(env, "native pthread callback java I am form native"); //����һ��jstring����
	(*env)->CallVoidMethod(env, gJavaObj , callbackID , str); //�ص��÷��������Ҵ��ݲ���ֵ

    (*gJavaVM)->DetachCurrentThread(gJavaVM);
}

void
NativePthreadCallbackJava_native(JNIEnv *env, jobject thiz)
{
	LOGD("JNI: master tid=%lu, pid=%lu", (unsigned long)gettid(), (unsigned long)getpid());
    //ע�⣬ֱ��ͨ������ȫ�ֵ�JNIEnv��jobject�������ڴ˱���env��thiz��ֵ�ǲ��������߳���ʹ�õ�

    //�̲߳�������env��������������JavaVMָ��������jvm���õģ����Կ���ͨ������ķ�������JavaVMָ�룬���߳���ʹ��
    (*env)->GetJavaVM(env,&gJavaVM);

    //ͬ��jobject����Ҳ���������߳��й��ã������Ҫ����ȫ�ֵ�jobject�������߳��з��ʸö���
    gJavaObj = (*env)->NewGlobalRef(env,thiz);

	//ͨ��pthread�ⴴ���߳�
	pthread_t threadId;
	 if( pthread_create(&threadId,NULL,native_thread_exec,NULL) != 0 ) {
		LOGE("native_thread_start pthread_create fail !");
		return;
	 }

	 LOGD("Create Pthread success");

	 pthread_join(threadId, NULL);

	 //��native���벻����Ҫ����һ��ȫ�����õ�ʱ��Ӧ�õ���DeleteGlobalRef���ͷ�����
	 (*env)->DeleteGlobalRef(env, gJavaObj);
	 gJavaObj = NULL;
}

jint
NativeCallLibrary_sum_native(JNIEnv *env, jobject thiz, jint a, jint b)
{
	return sum_Library(a, b);
}

/**
* ������Ӧ��
*/
static JNINativeMethod gMethods[] = {
	{"stringFromJNI", "()Ljava/lang/String;", (void*)stringFromJNI_native},
	{"PrimitiveTypes", "(ZBCSIJFD)I", (void*)PrimitiveTypes_native},
	{"ReferenceTypesSetByteArray1", "([B)V", ReferenceTypesSetByteArray1_native},
	{"ReferenceTypesSetByteArray2", "([B)V", ReferenceTypesSetByteArray2_native},
	{"ReferenceTypesSetDirectIntArray", "(Ljava/lang/Object;I)V", ReferenceTypesSetDirectIntArray_native},
	{"ReferenceTypesGetByteArray", "()[B", ReferenceTypesGetByteArray_native},
	{"OperateJavaAttribute", "()V", OperateJavaAttribute_native},
	{"NativeCallbackJava", "()V", NativeCallbackJava_native},
	{"NativeGetEmployeeInfo", "()Lcom/example/hellojni/Employee;", NativeGetEmployeeInfo_native},
	{"TransferEmployeeInfo", "(Lcom/example/hellojni/Employee;)V",TransferEmployeeInfo_native},
	{"NativePthreadCallbackJava", "()V", NativePthreadCallbackJava_native},
	{"NativeCallLibrary_sum", "(II)I", NativeCallLibrary_sum_native},
};

/*
* Ϊĳһ����ע�᱾�ط���
*/
static int registerNativeMethods(JNIEnv* env , const char* className,
		JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = (*env)->FindClass(env, className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/*
 **Ϊ������ע�᱾�ط���
 */
static int registerNatives(JNIEnv* env) {
	const char* kClassName = "com/example/hellojni/HelloJni";//ָ��Ҫע�����
	return registerNativeMethods(env, kClassName, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0]));
}

/*
 * * System.loadLibrary("lib")ʱ���� ����ɹ�����JNI�汾, ʧ�ܷ���-1
 */
//JNIEXPORT jint JNICALL
jint
JNI_OnLoad(JavaVM* vm, void* reserved) {
	 JNIEnv* env = NULL;
	 jint result = -1;
	 if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		 return -1;
	 }
	 if (!registerNatives(env)) {
		 return -1;
	 }
	 result = JNI_VERSION_1_4;
	 return result;
}
