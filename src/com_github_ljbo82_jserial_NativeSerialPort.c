/*
Copyright (c) 2023 Leandro José Britto de Oliveira

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "com_github_ljbo82_jserial_NativeSerialPort.h"

#include <serial.h>

#ifndef LIB_VERSION
	#warning "LIB_VERSION is not defined"
	#define LIB_VERSION "unknown"
#endif

#define __MIN(a,b) (a) < (b) ? (a) : (b)

JNIEXPORT jstring JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getNativeLibVersion(JNIEnv* env, jclass cls) {
	errno = 0;
	static jstring mVersion = NULL;
	if (!mVersion) {
		mVersion = (*env)->NewStringUTF(env, LIB_VERSION);
	}

	return mVersion;
}

#include <stdio.h>
JNIEXPORT jobjectArray JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getPortNames(JNIEnv* env, jclass cls) {
	errno = 0;
	static serial_list_t* mList = NULL;

	if (!mList) {
		mList = serial_list_new();
		if (!mList) {
			goto error;
		}
	}

	if (!serial_list_ports(mList))
		goto error;

	jobjectArray result = (*env)->NewObjectArray(env, serial_list_size(mList), (*env)->FindClass(env, "java/lang/String"), NULL);
	for (size_t i = 0; i < serial_list_size(mList); i++) {
		(*env)->SetObjectArrayElement(env, result, i, (*env)->NewStringUTF(env, serial_list_item(mList, i)));
	}

	return result;

error:
	return NULL;
}

JNIEXPORT jint JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getCurrentError(JNIEnv* env, jclass cls) {
	return errno;
}

JNIEXPORT void JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_clearCurrentError(JNIEnv* env, jclass cls) {
	errno = 0;
}

JNIEXPORT jlong JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_open(JNIEnv* env, jclass cls, jstring portName) {
	errno = 0;
	return (jlong)serial_open((*env)->GetStringUTFChars(env, portName, NULL));
}

JNIEXPORT jboolean JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_close(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	return serial_close((serial_t*)nativePort);
}

JNIEXPORT jboolean JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_config(JNIEnv* env, jclass cls, jlong nativePort, jint baud, jint dataBits, jint parity, jint stopBits) {
	errno = 0;
	serial_config_t cfg = {
		.baud     = baud,
		.dataBits = dataBits,
		.parity   = parity,
		.stopBits = stopBits
	};

	return serial_config((serial_t*)nativePort, &cfg);
}

JNIEXPORT jboolean JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_purge(JNIEnv* env, jclass cls, jlong nativePort, jint purgeType) {
	errno = 0;
	return serial_purge((serial_t*)nativePort, purgeType);
}

JNIEXPORT jint JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getBaud(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	serial_config_t cfg;
	serial_get_config((serial_t*)nativePort, &cfg);
	return cfg.baud;
}

JNIEXPORT jint JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getDataBits(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	serial_config_t cfg;
	serial_get_config((serial_t*)nativePort, &cfg);
	return cfg.dataBits;
}

JNIEXPORT jint JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getParity(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	serial_config_t cfg;
	serial_get_config((serial_t*)nativePort, &cfg);
	return cfg.parity;
}

JNIEXPORT jint JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getStopBits(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	serial_config_t cfg;
	serial_get_config((serial_t*)nativePort, &cfg);
	return cfg.stopBits;
}

JNIEXPORT jboolean JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_setReadTimeout(JNIEnv* env, jclass cls, jlong nativePort, jlong millis) {
	errno = 0;
	return serial_set_read_timeout((serial_t*)nativePort, millis);
}

JNIEXPORT jlong JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_getReadTimeout(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	return serial_get_read_timeout((serial_t*)nativePort);
}

JNIEXPORT jint JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_read(JNIEnv* env, jclass cls, jlong nativePort, jbyteArray out, jint off, jint len) {
	errno = 0;
	if (off < 0) {
		errno = SERIAL_ERROR_INVALID_PARAM;
		return -1;
	}

	if (len < 0) {
		errno = SERIAL_ERROR_INVALID_PARAM;
		return -1;
	}

	if (off + len > (*env)->GetArrayLength(env, out)) {
		errno = SERIAL_ERROR_INVALID_PARAM;
		return -1;
	}

	jbyte buffer[1024];
	int32_t mRead;
	uint32_t totalRead = 0;

	int previousError = errno;
	do {
		mRead = serial_read((serial_t*)nativePort, buffer, __MIN(sizeof(buffer), len));
		if (mRead > 0) {
			(*env)->SetByteArrayRegion(env, out, off, mRead, buffer);
			len -= mRead;
			off += mRead;
			totalRead += mRead;
		} else { // mRead <= 0 (error or timeout without error)
			if (totalRead) {
				// Ignore errors because some data was already read.
				// Error is raised only when no data was read.
				errno = previousError;
				return totalRead;
			} else {
				if (mRead < 0) { // Error
					return -1;
				} else { // Timeout (without error)
					break;
				}
			}
		}
	} while (len);

	return totalRead;
}

JNIEXPORT jboolean JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_write(JNIEnv* env, jclass cls, jlong nativePort, jbyteArray in, jint off, jint len) {
	errno = 0;
	if (off < 0) {
		errno = SERIAL_ERROR_INVALID_PARAM;
		return -1;
	}

	if (len < 0) {
		errno = SERIAL_ERROR_INVALID_PARAM;
		return -1;
	}

	if (off + len > (*env)->GetArrayLength(env, in)) {
		errno = SERIAL_ERROR_INVALID_PARAM;
		return -1;
	}

	jbyte mBuffer[1024];
	jsize remaining = len;
	jsize cursor = off;

	while (remaining > 0) {
		len = __MIN(remaining, sizeof(mBuffer));
		(*env)->GetByteArrayRegion(env, in, cursor, len, mBuffer);

		if (!serial_write((serial_t*)nativePort, mBuffer, len)) {
			return false;
		}

		remaining -= len;
		cursor += len;
	}

	return true;
}

JNIEXPORT jboolean JNICALL Java_com_github_ljbo82_jserial_NativeSerialPort_flush(JNIEnv* env, jclass cls, jlong nativePort) {
	errno = 0;
	return serial_flush((serial_t*)nativePort);
}
