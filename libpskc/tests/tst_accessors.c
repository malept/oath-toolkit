/*
 * tst_accessors.c - self-tests for libpskc accessor functions
 * Copyright (C) 2012 Simon Josefsson
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

#include "pskc.h"

#include <stdio.h>
#include <string.h>

const char *pskc_all =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<KeyContainer Version=\"42.17\" Id=\"KCID\""
  "              xmlns=\"urn:ietf:params:xml:ns:keyprov:pskc\">"
  "  <KeyPackage>"
  "    <DeviceInfo>"
  "      <Manufacturer>Manufacturer</Manufacturer>"
  "      <SerialNo>SerialNo</SerialNo>"
  "      <Model>Model</Model>"
  "      <IssueNo>IssueNo</IssueNo>"
  "      <DeviceBinding>DeviceBinding</DeviceBinding>"
  "      <StartDate>2006-05-04T01:02:03Z</StartDate>"
  "      <ExpiryDate>2012-01-09T23:50:51Z</ExpiryDate>"
  "      <UserId>DeviceUserId</UserId>"
  "    </DeviceInfo>"
  "    <CryptoModuleInfo>"
  "      <Id>CMID</Id>"
  "    </CryptoModuleInfo>"
  "    <Key Id=\"KID1\""
  "         Algorithm=\"urn:ietf:params:xml:ns:keyprov:pskc:hotp\">"
  "      <Issuer>Issuer</Issuer>"
  "      <AlgorithmParameters>"
  "    <Suite>Suite</Suite>"
#if 0
  "        <ChallengeFormat Encoding=\"DECIMAL\""
  "                         Min=\"42\""
  "                         Max=\"4711\""
  "                         CheckDigits=\"true\"/>"
#endif
  "        <ResponseFormat Encoding=\"DECIMAL\""
  "                        Length=\"8\""
  "                        CheckDigits=\"true\"/>"
  "      </AlgorithmParameters>"
  "      <KeyProfileId>KeyProfileId</KeyProfileId>"
  "      <KeyReference>KeyReference</KeyReference>"
  "      <FriendlyName>FriendlyName</FriendlyName>"
  "      <Data>"
  "        <Secret>"
  "          <PlainValue>"
  "            MTIzNDU2Nzg5MDEyMzQ1Njc4OTA="
  "          </PlainValue>"
  "        </Secret>"
  "        <Counter>"
  "          <PlainValue>12345678</PlainValue>"
  "        </Counter>"
  "        <Time>"
  "          <PlainValue>100</PlainValue>"
  "        </Time>"
  "        <TimeInterval>"
  "          <PlainValue>200</PlainValue>"
  "        </TimeInterval>"
  "        <TimeDrift>"
  "          <PlainValue>300</PlainValue>"
  "        </TimeDrift>"
  "      </Data>"
  "      <UserId>KeyUserId</UserId>"
  "      <Policy>"
  "        <StartDate>1950-01-01T00:00:00Z</StartDate>"
  "        <ExpiryDate>2006-05-31T00:00:00Z</ExpiryDate>"
  "        <PINPolicy PINKeyId=\"PKID\""
  "                   PINUsageMode=\"Local\""
  "                   MaxFailedAttempts=\"8\""
  "                   MinLength=\"99\""
  "                   MaxLength=\"999\""
  "                   PINEncoding=\"HEXADECIMAL\" />"
  "        <KeyUsage>OTP</KeyUsage>"
  "        <KeyUsage>CR</KeyUsage>"
  "        <NumberOfTransactions>1234</NumberOfTransactions>"
  "      </Policy>"
  "    </Key>"
  "  </KeyPackage>"
  "</KeyContainer>";

int
main (void)
{
  pskc_t *pskc;
  pskc_key_t *pskc_key;
  char *out;
  size_t len;
  int rc;

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_init: %d\n", rc);
      return 1;
    }

  rc = pskc_init (&pskc);
  if (rc != PSKC_OK)
    {
      printf ("pskc_init: %d\n", rc);
      return 1;
    }

  rc = pskc_parse_from_memory (pskc, strlen (pskc_all), pskc_all);
  if (rc != PSKC_OK)
    {
      printf ("pskc_parse_from_memory: %d\n", rc);
      return 1;
    }

  {
    const char *v = pskc_get_version (pskc);
    if (strcmp (v, "42.17") != 0)
      {
	printf ("pskc_get_version\n");
	return 1;
      }
  }

  {
    const char *i = pskc_get_id (pskc);
    if (strcmp (i, "KCID") != 0)
      {
	printf ("pskc_get_id\n");
	return 1;
      }
  }

  pskc_key = pskc_get_keypackage (pskc, 0);
  if (pskc_key == NULL)
    {
      printf ("pskc_get_keypackage\n");
      return 1;
    }

  {
    const char *m = pskc_get_device_manufacturer (pskc_key);
    if (strcmp (m, "Manufacturer") != 0)
      {
	printf ("pskc_get_device_manufacturer\n");
	return 1;
      }
  }

  {
    const char *s = pskc_get_device_serialno (pskc_key);
    if (strcmp (s, "SerialNo") != 0)
      {
	printf ("pskc_get_device_serialno\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_model (pskc_key);
    if (strcmp (m, "Model") != 0)
      {
	printf ("pskc_get_device_model\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_issueno (pskc_key);
    if (strcmp (m, "IssueNo") != 0)
      {
	printf ("pskc_get_device_issueno\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_devicebinding (pskc_key);
    if (strcmp (m, "DeviceBinding") != 0)
      {
	printf ("pskc_get_device_devicebinding\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_device_startdate (pskc_key);
    if (t->tm_sec != 3 || t->tm_min != 2 || t->tm_hour != 1 ||
	t->tm_mday != 4 || t->tm_mon != 4 || t->tm_year != 106)
      {
	printf ("pskc_get_device_startdate\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_device_expirydate (pskc_key);
    if (t->tm_sec != 51 || t->tm_min != 50 || t->tm_hour != 23 ||
	t->tm_mday != 9 || t->tm_mon != 0 || t->tm_year != 112)
      {
	printf ("pskc_get_device_expirydate\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_userid (pskc_key);
    if (strcmp (m, "DeviceUserId") != 0)
      {
	printf ("pskc_get_device_userid\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_cryptomodule_id (pskc_key);
    if (strcmp (m, "CMID") != 0)
      {
	printf ("pskc_get_cryptomodule_id\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_id (pskc_key);
    if (strcmp (m, "KID1") != 0)
      {
	printf ("pskc_get_key_id\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_algorithm (pskc_key);
    if (strcmp (m, "urn:ietf:params:xml:ns:keyprov:pskc:hotp") != 0)
      {
	printf ("pskc_get_key_algorithm\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_issuer (pskc_key);
    if (strcmp (m, "Issuer") != 0)
      {
	printf ("pskc_get_key_issuer\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_algparm_suite (pskc_key);
    if (strcmp (m, "Suite") != 0)
      {
	printf ("pskc_get_key_algparm_suite\n");
	return 1;
      }
  }

  {
    int p;
    pskc_valueformat v = pskc_get_key_algparm_resp_encoding (pskc_key, &p);
    if (p != 1 || v != PSKC_VALUEFORMAT_DECIMAL)
      {
	printf ("pskc_get_key_algparm_resp_encoding\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_algparm_resp_length (pskc_key, &p);
    if (p != 1 || l != 8)
      {
	printf ("pskc_get_key_algparm_resp_length\n");
	return 1;
      }
  }

  {
    int p;
    int b = pskc_get_key_algparm_resp_checkdigits (pskc_key, &p);
    if (p != 1 || b != 1)
      {
	printf ("pskc_get_key_algparm_resp_checkdigits\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_profileid (pskc_key);
    if (strcmp (m, "KeyProfileId") != 0)
      {
	printf ("pskc_get_key_profileid\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_reference (pskc_key);
    if (strcmp (m, "KeyReference") != 0)
      {
	printf ("pskc_get_key_reference\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_friendlyname (pskc_key);
    if (strcmp (m, "FriendlyName") != 0)
      {
	printf ("pskc_get_key_friendlyname\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_userid (pskc_key);
    if (strcmp (m, "KeyUserId") != 0)
      {
	printf ("pskc_get_key_userid\n");
	return 1;
      }
  }

  /* XXX
  extern PSKCAPI const char *pskc_get_key_data_secret (pskc_key_t *key,
						       size_t *len);
  */

  {
    int p;
    uint64_t l = pskc_get_key_data_counter (pskc_key, &p);
    if (p != 1 || l != 12345678)
      {
	printf ("pskc_get_key_data_counter\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_data_time (pskc_key, &p);
    if (p != 1 || l != 100)
      {
	printf ("pskc_get_key_data_time\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_data_timeinterval (pskc_key, &p);
    if (p != 1 || l != 200)
      {
	printf ("pskc_get_key_data_timeinterval\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_data_timedrift (pskc_key, &p);
    if (p != 1 || l != 300)
      {
	printf ("pskc_get_key_data_timedrift\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_key_policy_startdate (pskc_key);
    if (t->tm_sec != 0 || t->tm_min != 0 || t->tm_hour != 0 ||
	t->tm_mday != 1 || t->tm_mon != 0 || t->tm_year != 50)
      {
	printf ("pskc_get_key_policy_startdate\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_key_policy_expirydate (pskc_key);
    if (t->tm_sec != 0 || t->tm_min != 0 || t->tm_hour != 0 ||
	t->tm_mday != 31 || t->tm_mon != 4 || t->tm_year != 106)
      {
	printf ("pskc_get_key_policy_expirydate\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_policy_pinkeyid (pskc_key);
    if (strcmp (m, "PKID") != 0)
      {
	printf ("pskc_get_key_policy_pinkeyid\n");
	return 1;
      }
  }

  {
    int p;
    pskc_pinusagemode u = pskc_get_key_policy_pinusagemode (pskc_key, &p);
    if (p != 1 || u != PSKC_PINUSAGEMODE_LOCAL)
      {
	printf ("pskc_get_key_policy_pinusagemode\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_policy_pinmaxfailedattempts (pskc_key, &p);
    if (p != 1 || l != 8)
      {
	printf ("pskc_get_key_policy_pinmaxfailedattempts\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_policy_pinminlength (pskc_key, &p);
    if (p != 1 || l != 99)
      {
	printf ("pskc_get_key_policy_pinminlength\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_policy_pinmaxlength (pskc_key, &p);
    if (p != 1 || l != 999)
      {
	printf ("pskc_get_key_policy_pinmaxlength\n");
	return 1;
      }
  }

  {
    int p;
    pskc_valueformat v = pskc_get_key_policy_pinencoding (pskc_key, &p);
    if (p != 1 || v != PSKC_VALUEFORMAT_HEXADECIMAL)
      {
	printf ("pskc_get_key_policy_pinencoding\n");
	return 1;
      }
  }

  {
    int p;
    int v = pskc_get_key_policy_keyusages (pskc_key, &p);
    if (p != 1 || v != (PSKC_KEYUSAGE_OTP | PSKC_KEYUSAGE_CR))
      {
	printf ("pskc_get_key_policy_pinencoding\n");
	return 1;
      }
  }


  {
    int p;
    uint64_t l = pskc_get_key_policy_numberoftransactions (pskc_key, &p);
    if (p != 1 || l != 1234)
      {
	printf ("pskc_get_key_policy_numberoftransactions\n");
	return 1;
      }
  }

  rc = pskc_build_xml (pskc, &out, &len);
  if (rc != PSKC_OK)
    {
      printf ("pskc_build_xml: %d\n", rc);
      return 1;
    }

  pskc_free (out);

  pskc_done (pskc);

  rc = pskc_global_done ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_done: %d\n", rc);
      return 1;
    }

  return 0;
}
