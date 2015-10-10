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
 */
package com.example.hellojni;

import java.nio.ByteBuffer;
import android.app.Activity;
import android.util.Log;
import android.widget.TextView;
import android.os.Bundle;


public class HelloJni extends Activity
{
	
	private static final String TAG = "hello-jni";
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
        TextView  tv = new TextView(this);
        tv.setText( stringFromJNI() );
        setContentView(tv);
       
        //Primitive Types
        int ret;
        boolean a = true;
        byte b = 1;
        char c = 2;
        short d = 3;
        int e = 4;
        long f = 5;
        float h = 6.1f;
        double j = 6.2;
      
        ret = PrimitiveTypes(a, b, c, d, e, f, h, j);
        Log.d(TAG, "app PrimitiveTypes return:" + ret);
      
        //Reference Types Array
        byte array1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        byte array2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        ByteBuffer mDirectBuffer = ByteBuffer.allocateDirect(10);
        
        ReferenceTypesSetByteArray1(array1);
        Log.d(TAG, "app array1:");
        printBuffer(array1);
        ReferenceTypesSetByteArray2(array2);
        Log.d(TAG, "app array2:");
        printBuffer(array2);
        ReferenceTypesSetDirectIntArray(mDirectBuffer, 10);
        Log.d(TAG, "app DirectBuffer:");
        printBuffer(mDirectBuffer.array());
        byte[] buffer = ReferenceTypesGetByteArray();
        Log.d(TAG, "app GetArray:");
        printBuffer(buffer);
        
        //native operate java attribute and method
        Log.d(TAG, "app print original name: " + name);
        OperateJavaAttribute();
        Log.d(TAG, "app print new name: " + name);
        
        NativeCallbackJava();//Native层会调用Java_method()方法  
        
        //native operate java class
        Employee someone = NativeGetEmployeeInfo();//native层创建并返回java类
        Log.d(TAG, "app someone name:" + someone.getName());
        Log.d(TAG, "app someone age:" + someone.getAge());

        TransferEmployeeInfo(someone);//java向native层传递java类,并在native层打印出来。
        
        //native pthread callback java
        NativePthreadCallbackJava();//native层创建的线程去回调java
        
        //native call Library
        int sum = NativeCallLibrary_sum(2, 3);
        Log.d(TAG, "app 2+3=" + sum);
    }
    
    private String name = "Java layer";//类属性  
    
	    public void Java_method(String fromNative)   //Native层会调用Java_method()方法  
	    {
	    	Log.d(TAG, "app java function invoked by native function: " + fromNative);
	    }
    
	private void printBuffer( byte[] buffer )
	{
		StringBuffer sBuffer = new StringBuffer();
		for( int i=0; i<buffer.length; i++ ) {
			sBuffer.append(buffer[i]);
			sBuffer.append(" ");	
		}
		Log.d(TAG, "Native" + sBuffer.toString());
	}
    

    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    public native String  stringFromJNI();

    /* This is another native method declaration that is *not*
     * implemented by 'hello-jni'. This is simply to show that
     * you can declare as many native methods in your Java code
     * as you want, their implementation is searched in the
     * currently loaded native libraries only the first time
     * you call them.
     *
     * Trying to call this function will result in a
     * java.lang.UnsatisfiedLinkError exception !
     */
    public native String  unimplementedStringFromJNI();

    
    private native int PrimitiveTypes(boolean a, byte b, char c, short d, int e, long f, float h, double i);
    
    private native void ReferenceTypesSetByteArray1(byte[] array); 
    private native void ReferenceTypesSetByteArray2(byte[] array); 
    private native void ReferenceTypesSetDirectIntArray(Object buffer, int len);
    private native byte[] ReferenceTypesGetByteArray();
    
    private native void OperateJavaAttribute();
    private native void NativeCallbackJava();
    
    private native Employee NativeGetEmployeeInfo();
    private native void TransferEmployeeInfo(Employee someone);
    
    private native void NativePthreadCallbackJava();
    
    private native int NativeCallLibrary_sum(int a, int b);
    
    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
    	System.loadLibrary("sum");
        System.loadLibrary("hello-jni");
    }
}
