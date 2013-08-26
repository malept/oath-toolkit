/*
 * totp.c - implementation of the OATH TOTP algorithm
 * Copyright (C) 2011-2013 Simon Josefsson
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <config.h>

#include "oath.h"
#include "hotp.h"
#include "aux.h"		/* _oath_strcmp_callback */

/**
 * oath_totp_generate:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to compute TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @digits: number of requested digits in the OTP, excluding checksum
 * @output_otp: output buffer, must have room for the output OTP plus zero
 *
 * Generate a one-time-password using the time-variant TOTP algorithm
 * described in RFC 6238.  The input parameters are taken as time
 * values.
 *
 * The system parameter @time_step_size describes how long the time
 * window for each OTP is.  The recommended value is 30 seconds, and
 * you can use the value 0 or the symbol
 * %OATH_TOTP_DEFAULT_TIME_STEP_SIZE to indicate this.
 *
 * The system parameter @start_offset denote the Unix time when time
 * steps are started to be counted.  The recommended value is 0, to
 * fall back on the Unix epoch) and you can use the symbol
 * %OATH_TOTP_DEFAULT_START_TIME to indicate this.
 *
 * The @output_otp buffer must have room for at least @digits
 * characters, plus one for the terminating NUL.
 *
 * Currently only values 6, 7 and 8 for @digits are supported.  This
 * restriction may be lifted in future versions.
 *
 * Returns: On success, %OATH_OK (zero) is returned, otherwise an
 *   error code is returned.
 *
 * Since: 1.4.0
 **/
int
oath_totp_generate (const char *secret,
		    size_t secret_length,
		    time_t now,
		    unsigned time_step_size,
		    time_t start_offset, unsigned digits, char *output_otp)
{
  return oath_totp_generate2 (secret, secret_length, now, time_step_size,
			      start_offset, digits, 0, output_otp);
}

/**
 * oath_totp_generate2:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to compute TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @digits: number of requested digits in the OTP, excluding checksum
 * @flags: flags indicating mode, one of #oath_totp_flags
 * @output_otp: output buffer, must have room for the output OTP plus zero
 *
 * Generate a one-time-password using the time-variant TOTP algorithm
 * described in RFC 6238.  The input parameters are taken as time
 * values.
 *
 * The system parameter @time_step_size describes how long the time
 * window for each OTP is.  The recommended value is 30 seconds, and
 * you can use the value 0 or the symbol
 * %OATH_TOTP_DEFAULT_TIME_STEP_SIZE to indicate this.
 *
 * The system parameter @start_offset denote the Unix time when time
 * steps are started to be counted.  The recommended value is 0, to
 * fall back on the Unix epoch) and you can use the symbol
 * %OATH_TOTP_DEFAULT_START_TIME to indicate this.
 *
 * The @output_otp buffer must have room for at least @digits
 * characters, plus one for the terminating NUL.
 *
 * Currently only values 6, 7 and 8 for @digits are supported.  This
 * restriction may be lifted in future versions.
 *
 * The @flags parameter may be used to change the MAC function, for
 * example %OATH_TOTP_HMAC_SHA256 or %OATH_TOTP_HMAC_SHA512.
 *
 * Returns: On success, %OATH_OK (zero) is returned, otherwise an
 *   error code is returned.
 *
 * Since: 2.6.0
 **/
int
oath_totp_generate2 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     unsigned digits, int flags, char *output_otp)
{
  uint64_t nts;

  if (time_step_size == 0)
    time_step_size = OATH_TOTP_DEFAULT_TIME_STEP_SIZE;

  nts = (now - start_offset) / time_step_size;

  return _oath_hotp_generate2 (secret,
			       secret_length,
			       nts,
			       digits,
			       false, OATH_HOTP_DYNAMIC_TRUNCATION, flags,
			       output_otp);
}

/**
 * oath_totp_validate:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to validate TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @window: how many OTPs after/before start OTP to test
 * @otp: the OTP to validate.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 1.6.0
 **/
int
oath_totp_validate (const char *secret,
		    size_t secret_length,
		    time_t now,
		    unsigned time_step_size,
		    time_t start_offset, size_t window, const char *otp)
{
  return oath_totp_validate3 (secret, secret_length, now, time_step_size,
			      start_offset, window, NULL, NULL, otp);
}

/**
 * oath_totp_validate_callback:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to compute TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @window: how many OTPs after start counter to test
 * @digits: number of requested digits in the OTP
 * @strcmp_otp: function pointer to a strcmp-like function.
 * @strcmp_handle: caller handle to be passed on to @strcmp_otp.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Validation is implemented by generating a number of potential OTPs
 * and performing a call to the @strcmp_otp function, to compare the
 * potential OTP against the given @otp.  It has the following
 * prototype:
 *
 * int (*oath_validate_strcmp_function) (void *handle, const char *test_otp);
 *
 * The function should be similar to strcmp in that it return 0 only
 * on matches.  It differs by permitting use of negative return codes
 * as indication of internal failures in the callback.  Positive
 * values indicate OTP mismatch.
 *
 * This callback interface is useful when you cannot compare OTPs
 * directly using normal strcmp, but instead for example only have a
 * hashed OTP.  You would then typically pass in the hashed OTP in the
 * @strcmp_handle and let your implementation of @strcmp_otp hash the
 * test_otp OTP using the same hash, and then compare the results.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns position in OTP window (zero is first position),
 *   or %OATH_INVALID_OTP if no OTP was found in OTP window, or an
 *   error code.
 *
 * Since: 1.6.0
 **/
int
oath_totp_validate_callback (const char *secret,
			     size_t secret_length,
			     time_t now,
			     unsigned time_step_size,
			     time_t start_offset,
			     unsigned digits,
			     size_t window,
			     oath_validate_strcmp_function strcmp_otp,
			     void *strcmp_handle)
{
  return oath_totp_validate4_callback (secret, secret_length, now,
				       time_step_size, start_offset,
				       digits, window, NULL, NULL, 0,
				       strcmp_otp, strcmp_handle);
}

/**
 * oath_totp_validate2:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to validate TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @window: how many OTPs after/before start OTP to test
 * @otp_pos: output search position in search window (may be NULL).
 * @otp: the OTP to validate.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 1.10.0
 **/
int
oath_totp_validate2 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     size_t window, int *otp_pos, const char *otp)
{
  return oath_totp_validate4_callback (secret, secret_length, now,
				       time_step_size, start_offset,
				       strlen (otp), window, otp_pos,
				       NULL, 0, _oath_strcmp_callback,
				       (void *) otp);
}

/**
 * oath_totp_validate2_callback:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to compute TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @digits: number of requested digits in the OTP
 * @window: how many OTPs after start counter to test
 * @otp_pos: output search position in search window (may be NULL).
 * @strcmp_otp: function pointer to a strcmp-like function.
 * @strcmp_handle: caller handle to be passed on to @strcmp_otp.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Validation is implemented by generating a number of potential OTPs
 * and performing a call to the @strcmp_otp function, to compare the
 * potential OTP against the given @otp.  It has the following
 * prototype:
 *
 * int (*oath_validate_strcmp_function) (void *handle, const char *test_otp);
 *
 * The function should be similar to strcmp in that it return 0 only
 * on matches.  It differs by permitting use of negative return codes
 * as indication of internal failures in the callback.  Positive
 * values indicate OTP mismatch.
 *
 * This callback interface is useful when you cannot compare OTPs
 * directly using normal strcmp, but instead for example only have a
 * hashed OTP.  You would then typically pass in the hashed OTP in the
 * @strcmp_handle and let your implementation of @strcmp_otp hash the
 * test_otp OTP using the same hash, and then compare the results.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 1.10.0
 **/
int
oath_totp_validate2_callback (const char *secret,
			      size_t secret_length,
			      time_t now,
			      unsigned time_step_size,
			      time_t start_offset,
			      unsigned digits,
			      size_t window,
			      int *otp_pos,
			      oath_validate_strcmp_function strcmp_otp,
			      void *strcmp_handle)
{
  return oath_totp_validate4_callback (secret, secret_length, now,
				       time_step_size, start_offset,
				       digits, window, otp_pos, NULL,
				       0, strcmp_otp, strcmp_handle);
}

/**
 * oath_totp_validate3:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to validate TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @window: how many OTPs after/before start OTP to test
 * @otp_pos: output search position in search window (may be NULL).
 * @otp_counter: counter value used to calculate OTP value (may be NULL).
 * @otp: the OTP to validate.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 2.4.0
 **/
int
oath_totp_validate3 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     size_t window,
		     int *otp_pos, uint64_t * otp_counter, const char *otp)
{
  return oath_totp_validate4_callback (secret, secret_length, now,
				       time_step_size, start_offset,
				       strlen (otp), window, otp_pos,
				       otp_counter, 0, _oath_strcmp_callback,
				       (void *) otp);
}

/**
 * oath_totp_validate3_callback:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to compute TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @digits: number of requested digits in the OTP
 * @window: how many OTPs after start counter to test
 * @otp_pos: output search position in search window (may be NULL).
 * @otp_counter: counter value used to calculate OTP value (may be NULL).
 * @strcmp_otp: function pointer to a strcmp-like function.
 * @strcmp_handle: caller handle to be passed on to @strcmp_otp.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Validation is implemented by generating a number of potential OTPs
 * and performing a call to the @strcmp_otp function, to compare the
 * potential OTP against the given @otp.  It has the following
 * prototype:
 *
 * int (*oath_validate_strcmp_function) (void *handle, const char *test_otp);
 *
 * The function should be similar to strcmp in that it return 0 only
 * on matches.  It differs by permitting use of negative return codes
 * as indication of internal failures in the callback.  Positive
 * values indicate OTP mismatch.
 *
 * This callback interface is useful when you cannot compare OTPs
 * directly using normal strcmp, but instead for example only have a
 * hashed OTP.  You would then typically pass in the hashed OTP in the
 * @strcmp_handle and let your implementation of @strcmp_otp hash the
 * test_otp OTP using the same hash, and then compare the results.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 2.4.0
 **/
int
oath_totp_validate3_callback (const char *secret,
			      size_t secret_length,
			      time_t now,
			      unsigned time_step_size,
			      time_t start_offset,
			      unsigned digits,
			      size_t window,
			      int *otp_pos,
			      uint64_t * otp_counter,
			      oath_validate_strcmp_function strcmp_otp,
			      void *strcmp_handle)
{
  return oath_totp_validate4_callback (secret, secret_length, now,
				       time_step_size, start_offset,
				       digits, window, otp_pos,
				       otp_counter, 0, strcmp_otp,
				       strcmp_handle);
}

/**
 * oath_totp_validate4:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to validate TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @window: how many OTPs after/before start OTP to test
 * @otp_pos: output search position in search window (may be NULL).
 * @otp_counter: counter value used to calculate OTP value (may be NULL).
 * @flags: flags indicating mode, one of #oath_totp_flags
 * @otp: the OTP to validate.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * The @flags parameter may be used to change the MAC function, for
 * example %OATH_TOTP_HMAC_SHA256 or %OATH_TOTP_HMAC_SHA512.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 2.6.0
 **/
int
oath_totp_validate4 (const char *secret,
		     size_t secret_length,
		     time_t now,
		     unsigned time_step_size,
		     time_t start_offset,
		     size_t window,
		     int *otp_pos,
		     uint64_t * otp_counter, int flags, const char *otp)
{
  return oath_totp_validate4_callback (secret, secret_length, now,
				       time_step_size, start_offset,
				       strlen (otp), window, otp_pos,
				       otp_counter, flags,
				       _oath_strcmp_callback, (void *) otp);
}

/**
 * oath_totp_validate4_callback:
 * @secret: the shared secret string
 * @secret_length: length of @secret
 * @now: Unix time value to compute TOTP for
 * @time_step_size: time step system parameter (typically 30)
 * @start_offset: Unix time of when to start counting time steps (typically 0)
 * @digits: number of requested digits in the OTP
 * @window: how many OTPs after start counter to test
 * @otp_pos: output search position in search window (may be NULL).
 * @otp_counter: counter value used to calculate OTP value (may be NULL).
 * @flags: flags indicating mode, one of #oath_totp_flags
 * @strcmp_otp: function pointer to a strcmp-like function.
 * @strcmp_handle: caller handle to be passed on to @strcmp_otp.
 *
 * Validate an OTP according to OATH TOTP algorithm per RFC 6238.
 *
 * Validation is implemented by generating a number of potential OTPs
 * and performing a call to the @strcmp_otp function, to compare the
 * potential OTP against the given @otp.  It has the following
 * prototype:
 *
 * int (*oath_validate_strcmp_function) (void *handle, const char *test_otp);
 *
 * The function should be similar to strcmp in that it return 0 only
 * on matches.  It differs by permitting use of negative return codes
 * as indication of internal failures in the callback.  Positive
 * values indicate OTP mismatch.
 *
 * This callback interface is useful when you cannot compare OTPs
 * directly using normal strcmp, but instead for example only have a
 * hashed OTP.  You would then typically pass in the hashed OTP in the
 * @strcmp_handle and let your implementation of @strcmp_otp hash the
 * test_otp OTP using the same hash, and then compare the results.
 *
 * Currently only OTP lengths of 6, 7 or 8 digits are supported.  This
 * restrictions may be lifted in future versions, although some
 * limitations are inherent in the protocol.
 *
 * The @flags parameter may be used to change the MAC function, for
 * example %OATH_TOTP_HMAC_SHA256 or %OATH_TOTP_HMAC_SHA512.
 *
 * Returns: Returns absolute value of position in OTP window (zero is
 *   first position), or %OATH_INVALID_OTP if no OTP was found in OTP
 *   window, or an error code.
 *
 * Since: 2.6.0
 **/
int
oath_totp_validate4_callback (const char *secret,
			      size_t secret_length,
			      time_t now,
			      unsigned time_step_size,
			      time_t start_offset,
			      unsigned digits,
			      size_t window,
			      int *otp_pos,
			      uint64_t * otp_counter,
			      int flags,
			      oath_validate_strcmp_function strcmp_otp,
			      void *strcmp_handle)
{
  unsigned iter = 0;
  char tmp_otp[10];
  int rc;
  uint64_t nts;

  if (time_step_size == 0)
    time_step_size = OATH_TOTP_DEFAULT_TIME_STEP_SIZE;

  nts = (now - start_offset) / time_step_size;

  do
    {
      rc = _oath_hotp_generate2 (secret,
				 secret_length,
				 nts + iter,
				 digits,
				 false,
				 OATH_HOTP_DYNAMIC_TRUNCATION,
				 flags, tmp_otp);
      if (rc != OATH_OK)
	return rc;

      if ((rc = strcmp_otp (strcmp_handle, tmp_otp)) == 0)
	{
	  if (otp_counter)
	    *otp_counter = nts + iter;
	  if (otp_pos)
	    *otp_pos = iter;
	  return iter;
	}
      if (rc < 0)
	return OATH_STRCMP_ERROR;

      if (iter > 0)
	{
	  rc = _oath_hotp_generate2 (secret,
				     secret_length,
				     nts - iter,
				     digits,
				     false,
				     OATH_HOTP_DYNAMIC_TRUNCATION,
				     flags, tmp_otp);
	  if (rc != OATH_OK)
	    return rc;

	  if ((rc = strcmp_otp (strcmp_handle, tmp_otp)) == 0)
	    {
	      if (otp_counter)
		*otp_counter = nts - iter;
	      if (otp_pos)
		*otp_pos = -iter;
	      return iter;
	    }
	  if (rc < 0)
	    return OATH_STRCMP_ERROR;
	}
    }
  while (window - iter++ > 0);

  return OATH_INVALID_OTP;
}
