/* 
 * C
 * 
 * Copyright 2011-2014 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */
/* 
 * This file is auto-generated - DO NOT EDIT IT
 */
/* 
 * Implementation header file. Shall only be included by client implementation C files.
 */
#include <stdint.h>
#include <intern/LLMJVM_impl.h>
// --------------------------------------------------------------------------------
// -                                  Constants                                   -
// --------------------------------------------------------------------------------

/**
 * Returned value when function call has succeeded.
 */
#define LLMJVM_OK (0)

/**
 * Returned value when current task has been interrupted during function call.
 */
#define LLMJVM_INTERRUPTED (1)

/**
 * Returned value when function call has failed.
 */
#define LLMJVM_ERROR (-1)

// --------------------------------------------------------------------------------
// -                      Functions that must be implemented                      -
// --------------------------------------------------------------------------------

/**
 * This function is called once during MicroJvm virtual machine creation (i.e. when the <code>SNI_createVM()</code>
 * function is called). It may be used to initialize specific data.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_initialize(void);

/**
 * This function is called once during the MicroJvm virtual machine startup by the MicroJvm virtual machine task (i.e.
 * when the <code>SNI_startVM()</code> function is called). It can be useful if the MicroJvm virtual machine support
 * needs to know the MicroJvm virtual machine task.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_vmTaskStarted(void);

/**
 * Schedule an alarm (or timer) that will be triggered at the given absolute (system) time.
 * If an alarm is already scheduled for an earlier time this function must do nothing,
 * otherwise it must configure the alarm. If the given absolute time has already been reached, this
 * function must call {@link LLMJVM_schedule}. Previously scheduled alarm
 * must be canceled, only one alarm is scheduled at the same time.
 * The scheduled alarm must call the function {@link LLMJVM_schedule} when it is triggered.
 * The specified time is in milliseconds.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_scheduleRequest(int64_t absoluteTime);

/**
 * Causes the MicroJvm virtual machine RTOS task to sleep until it is woken up by the {@link LLMJVM_wakeupVM} function.
 * This function is called by the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} if wakeup occurred, {@link LLMJVM_INTERRUPTED} if the MicroJvm virtual machine task
 * has been interrupted, or {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_idleVM(void);

/**
 * Wake up the MicroJvm virtual machine RTOS task. If the MicroJvm virtual machine task is not sleeping,
 * the wakeup stays pending and the MicroJvm virtual machine will not sleep on the next {@link LLMJVM_idleVM} call unless
 * there is a call to {@link LLMJVM_ackWakeup} between this call and the next {@link LLMJVM_idleVM} call.
 * This function must be called only by the MicroJvm virtual machine code. If a task wants to wake up the MicroJvm
 * virtual machine, it must use the {@link LLMJVM_schedule} function (which may in turn call
 * this function).
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_wakeupVM(void);

/**
 * Clears any outstanding {@link LLMJVM_wakeupVM} request. After calling this function, a call to {@link LLMJVM_idleVM} will
 * result in a wait even if {@link LLMJVM_wakeupVM} has been called previously (provided no other {@link LLMJVM_wakeupVM} call
 * has occurred since the call to this function. This function must cancel the alarm previously scheduled with
 * {@link LLMJVM_scheduleRequest}.
 * This function is called by the MicroJvm virtual machine task.
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_ackWakeup(void);

/**
 * Returns the ID of the current OS task.<br>
 * This function may be called within the MicroJvm virtual machine task or another OS task.
 */
int32_t LLMJVM_IMPL_getCurrentTaskID(void);

/**
 * This function is called during MicroJvm virtual machine end (i.e. at the end of the <code>SNI_startVM()</code> function).
 * It may be used to freed specific data.<br>
 * @return {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_IMPL_shutdown(void);

/**
 * Sets the application time. The application time is the difference, measured in milliseconds,
 * between the current time and midnight, January 1, 1970 UTC.
 * This time does not change the system time.
 * @param t the application time to set in milliseconds
 */
void LLMJVM_IMPL_setApplicationTime(int64_t t);

/**
 * Gets the system or the application time in milliseconds.
 * The system time is the running time since the start of the device. This time is independent from any user considerations
 * and cannot be changed.
 * The application time is the difference, measured in milliseconds, between the current time and midnight, January 1, 1970 UTC.
 * @param system if 1, get the system time, otherwise get the application time.
 */
int64_t LLMJVM_IMPL_getCurrentTime(uint8_t system);

/**
 * Gets the current timestamp in nanoseconds.
 * Only elapsed time between two calls is meaningful.
 * @return the current timestamp in nanoseconds
 */
int64_t LLMJVM_IMPL_getTimeNanos(void);

// --------------------------------------------------------------------------------
// -          Functions provided by the library - not to be implemented           -
// --------------------------------------------------------------------------------

/**
 * Notify the MicroJvm virtual machine that it must wake up or schedule a new thread.
 * @return  {@link LLMJVM_OK} on success, {@link LLMJVM_ERROR} on error.
 */
int32_t LLMJVM_schedule(void);
