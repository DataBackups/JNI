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

	//直接将Java端的数组拷贝到本地的数据中，建议使用这种方式，更加安全
	(*env)->GetByteArrayRegion(env, buffer, 0, len, array);

	//可以通过array来访问这段数组的值了,注意，修改的只是本地的值，Java端不会同时改变数组的值
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

	//将本地指针指向含有Java端数组的内存地址，依赖Jvm的具体实现，可能是锁住Java端的那段数组不被回收（增加引用计数），
	//也可能所Jvm在堆上对该数组的一份拷贝
	//速度和效率比GetByteArrayRegion方法要高很多
	char * pBuffer = (*env)->GetByteArrayElements(env,buffer,NULL);
	if( pBuffer == NULL ) {
		LOGE("GetIntArrayElements Failed!");
		return;
	}

	for (i=0; i < len; i++) {
		array[i] = pBuffer[i];
		LOGD("JNI: pBuffer[%d]=%d", i, pBuffer[i]);
	}

	//可以通过pBuffer指针来访问这段数组的值了,注意，修改的是堆上的值，Java端可能会同步改变，依赖于Jvm的具体实现,不建议通过本方法改变Java端的数组值
	for( i=0; i<len; i++ ) {
		pBuffer[i] = i + 10;
	}

	//最后不要忘记释放指针（减小引用计数）
	(*env)->ReleaseByteArrayElements(env,buffer,pBuffer,0);
}

void
ReferenceTypesSetDirectIntArray_native(JNIEnv *env, jobject thiz, jbyteArray buffer, int len)
{
	//无需拷贝，直接获取与Java端共享的直接内存地址(效率比较高，但object的构造析构开销大，建议长期使用的大型buffer采用这种方式)
	unsigned char * pBuffer = (unsigned char *)(*env)->GetDirectBufferAddress(env,buffer);
	if( pBuffer == NULL ) {
		LOGE("GetDirectBufferAddress Failed!");
		return;
	}

	//可以通过pBuffer指针来访问这段数组的值了,注意，修改数组的值后，Java端同时变化
	int i=0;
	for( i=0; i<len; i++ ) {
		pBuffer[i] = i;
	}
}

jbyteArray
ReferenceTypesGetByteArray_native(JNIEnv *env, jobject thiz)
{
	//传递JNI层的数组数据到Java端，有两种方法，一种是本例所示的通过返回值来传递
	//另一种是通过回调Java端的函数来传递(多用于jni线程中回调java层)
	unsigned char buffer[TEST_BUFFER_SIZE];

	int i=0;
	for( i=0; i<TEST_BUFFER_SIZE; i++ ) {
		buffer[i] = i;
	}

	//分配ByteArray
	jbyteArray array = (*env)->NewByteArray(env,TEST_BUFFER_SIZE);

	//将传递数据拷贝到java端
	(*env)->SetByteArrayRegion(env, array, 0, TEST_BUFFER_SIZE, buffer);

	return array;
}

void
OperateJavaAttribute_native(JNIEnv *env, jobject thiz)
{
	 //获得jfieldID 以及 该字段的初始值
	jfieldID  nameFieldId ;
	jclass cls = (*env)->GetObjectClass(env, thiz);  //获得Java层该对象实例的类引用，即HelloJNI类引用
	nameFieldId = (*env)->GetFieldID(env, cls , "name" , "Ljava/lang/String;"); //获得属性句柄
	if(nameFieldId == NULL)
	{
		LOGE("can not find java field id");
	}

	jstring javaNameStr = (jstring)(*env)->GetObjectField(env, thiz ,nameFieldId);  // 获得该属性的值
	const char * c_javaName = (*env)->GetStringUTFChars(env, javaNameStr , NULL);  //转换为 char *类型
	LOGD("JNI: java name is: %s", c_javaName);
	(*env)->ReleaseStringUTFChars(env, javaNameStr , c_javaName);  //释放局部引用

	//构造一个jString对象
	char * c_ptr_name = "Native layer";
	jstring cName = (*env)->NewStringUTF(env, c_ptr_name); //构造一个jstring对象
	(*env)->SetObjectField(env, thiz , nameFieldId , cName); // 设置该字段的值

}

void
NativeCallbackJava_native(JNIEnv *env, jobject thiz)
{
	 //回调Java中的方法
	jclass cls = (*env)->GetObjectClass(env, thiz);//获得Java类实例
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "Java_method" , "(Ljava/lang/String;)V") ;//获得该回调方法句柄

	if(callbackID == NULL)
	{
		LOGE("getMethodId is failed \n");
	}

	jstring native_desc = (*env)->NewStringUTF(env, "callback java I am form Native");
	(*env)->CallVoidMethod(env, thiz , callbackID , native_desc); //回调该方法，并且传递参数值
}

jobject
NativeGetEmployeeInfo_native(JNIEnv *env, jobject thiz)
{
	LOGD("JNI invoke NativeGetEmployeeInfo_native");

	jclass employeecls = (*env)->FindClass(env, "com/example/hellojni/Employee");

	//获得得该类型的构造函数  函数名为 <init> 返回类型必须为 void 即 V
	//注意：该 ID特指该类class的构造函数ID ， 必须通过调用 GetMethodID() 获得，且调用时的方法名必须为 <init>，而返回类型必须为 void (V)
	jmethodID constrocMID = (*env)->GetMethodID(env, employeecls, "<init>", "(ILjava/lang/String;)V");

	jstring str = (*env)->NewStringUTF(env, "xiaoming");

	jobject employeeobj = (*env)->NewObject(env, employeecls,constrocMID, 30, str);  //构造一个对象，调用该类的构造函数，并且传递参数

	return employeeobj;
}

void
TransferEmployeeInfo_native(JNIEnv *env, jobject thiz, jobject employeeobj)
{
	LOGD("JNI invoke TransferEmployeeInfo_native");

    jclass employeecls = (*env)->GetObjectClass(env, employeeobj); //或得Student类引用

    if(employeecls == NULL)
    {
        LOGE("JNI GetObjectClass failed");
    }

    jfieldID ageFieldID = (*env)->GetFieldID(env, employeecls,"age","I"); //获得得Student类的属性id
    jfieldID nameFieldID = (*env)->GetFieldID(env, employeecls,"name","Ljava/lang/String;"); // 获得属性ID

    jint age = (*env)->GetIntField(env, employeeobj , ageFieldID);  //获得属性值
    jstring name = (jstring)(*env)->GetObjectField(env, employeeobj , nameFieldID);//获得属性值

    const char * c_name = (*env)->GetStringUTFChars(env, name ,NULL);//转换成 char *

    LOGD("JNI name:%s, age:%d", c_name, age);
    (*env)->ReleaseStringUTFChars(env, name,c_name); //释放引用
    //LOGD("JNI name:%s, age:%d", c_name, age); 不能再释放后在调用
}

JavaVM * gJavaVM;
jobject  gJavaObj;

static void* native_thread_exec(void *arg) {

    JNIEnv *env;
    LOGD("JNI: pthread tid=%lu, pid=%lu", (unsigned long)gettid(), (unsigned long)getpid());
    //从全局的JavaVM中获取到环境变量
    (*gJavaVM)->AttachCurrentThread(gJavaVM,&env, NULL);

 //   //获取Java层对应的类
    jclass cls = (*env)->GetObjectClass(env,gJavaObj);
    if( cls == NULL ) {
    	LOGE("JNI Fail to find javaClass");
    	return 0;
    }

//    //获取Java层被回调的函数
    jmethodID callbackID = (*env)->GetMethodID(env, cls, "Java_method" , "(Ljava/lang/String;)V") ;//获得该回调方法句柄
	if(callbackID == NULL)
	{
		LOGE("getMethodId is failed \n");
	}

	jstring str = (*env)->NewStringUTF(env, "native pthread callback java I am form native"); //构造一个jstring对象
	(*env)->CallVoidMethod(env, gJavaObj , callbackID , str); //回调该方法，并且传递参数值

    (*gJavaVM)->DetachCurrentThread(gJavaVM);
}

void
NativePthreadCallbackJava_native(JNIEnv *env, jobject thiz)
{
	LOGD("JNI: master tid=%lu, pid=%lu", (unsigned long)gettid(), (unsigned long)getpid());
    //注意，直接通过定义全局的JNIEnv和jobject变量，在此保存env和thiz的值是不可以在线程中使用的

    //线程不允许共用env环境变量，但是JavaVM指针是整个jvm共用的，所以可以通过下面的方法保存JavaVM指针，在线程中使用
    (*env)->GetJavaVM(env,&gJavaVM);

    //同理，jobject变量也不允许在线程中共用，因此需要创建全局的jobject对象在线程中访问该对象
    gJavaObj = (*env)->NewGlobalRef(env,thiz);

	//通过pthread库创建线程
	pthread_t threadId;
	 if( pthread_create(&threadId,NULL,native_thread_exec,NULL) != 0 ) {
		LOGE("native_thread_start pthread_create fail !");
		return;
	 }

	 LOGD("Create Pthread success");

	 pthread_join(threadId, NULL);

	 //在native代码不再需要访问一个全局引用的时候，应该调用DeleteGlobalRef来释放它。
	 (*env)->DeleteGlobalRef(env, gJavaObj);
	 gJavaObj = NULL;
}

jint
NativeCallLibrary_sum_native(JNIEnv *env, jobject thiz, jint a, jint b)
{
	return sum_Library(a, b);
}

/**
* 方法对应表
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
* 为某一个类注册本地方法
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
 **为所有类注册本地方法
 */
static int registerNatives(JNIEnv* env) {
	const char* kClassName = "com/example/hellojni/HelloJni";//指定要注册的类
	return registerNativeMethods(env, kClassName, gMethods,
			sizeof(gMethods) / sizeof(gMethods[0]));
}

/*
 * * System.loadLibrary("lib")时调用 如果成功返回JNI版本, 失败返回-1
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
