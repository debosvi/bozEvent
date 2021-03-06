/*
The MIT License (MIT)

Copyright (c) 2013 Vincent de RIBOU <belzo2005-dolphin@yahoo.fr>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*!
 * \file        bevt_client.h
 * \brief       BozEvent client library public APIs.
 * \version     @boz_msg_VERSION@
 * \date        2013/01/14
 * \author      Vincent de RIBOU.
 * \copyright   Aquaplouf Land.
 *
 * \brief Declares functions and types to be managed by bozEvent client endpoint.
 *
 */
#ifndef _BOZ_EVENT_CLIENT_H_
#define _BOZ_EVENT_CLIENT_H_

/**
 * \ingroup BOZCORE
 * \defgroup BOZMSG Message Management public APIs.
 * \{
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @brief Maximum data size to be sent as event.
 */
#define BEVT_MAX_DATA_SIZE (1024)

/**
 * @brief bozEvent client identifier.
 */
typedef uint64_t bevt_client_id_t;

/**
 * @brief bozEvent client identifier priority level.
 */
typedef enum {
    BEVT_CLIENT_PRIO_DEFAULT=0,
    BEVT_CLIENT_PRIO_MINOR,
    BEVT_CLIENT_PRIO_MEDIUM,
    BEVT_CLIENT_PRIO_MAJOR
} bevt_client_prio_t;

/**
 * @brief Invalid message identifier value.
 */
#define BEVT_CLIENT_ID_INVALID (-1)

/**
 * @brief Initialise bozEvent client endpoint.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_init(void);

/**
 * @brief Finalise bozEvent client endpoint.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_finalise(void);

/**
 * @brief Process bozEvent client endpoint.
 * @param[in]   timeout in milliseconds.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_process(const int to_ms);

/**
 * @brief Register to a specific event identifier.
 * @param[in]   id event identifier.
 * @param[in]   event priority management.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_register(const bevt_client_id_t, const bevt_client_prio_t prio);

/**
 * @brief Register to a specific event identifier (update only).
 * @param[in]   id event identifier.
 * @param[in]   event priority management.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_register_update(const bevt_client_id_t, const bevt_client_prio_t prio);

/**
 * @brief Unregister from a specific event identifier.
 * @param[in]   id event identifier.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_unregister(const bevt_client_id_t);

/**
 * @brief Subscribe to a specific event identifier.
 * @param[in]   id event identifier.
 * @param[in]   event priority management.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_subscribe(const bevt_client_id_t, const bevt_client_prio_t prio);

/**
 * @brief Subscribe to a specific event identifier (update only).
 * @param[in]   id event identifier.
 * @param[in]   event priority management.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_subscribe_update(const bevt_client_id_t, const bevt_client_prio_t prio);

/**
 * @brief Unsubscribe from a specific event identifier.
 * @param[in]   id event identifier.
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_unsubscribe(const bevt_client_id_t);

/**
 * @brief Notify to a specific event identifier.
 * @param[in]   id event identifier.
 * @param[in]   data buffer reference
 * @param[in]   len buffer length
 * @return      error code.
 * @retval      0 in case of success.
 * @retval      -1 in case of failure, errno set accordingly.
 */ 
int bevt_client_notify(const bevt_client_id_t, const char const *data, const unsigned int len);

/**
 *\}
 * End of group
 */

#ifdef __cplusplus
}
#endif

#endif   // _BOZ_MSG_H_ 
