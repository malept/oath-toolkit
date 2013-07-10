/*
 * tst_setters.c - self-tests for libpskc field setter functions
 * Copyright (C) 2013 Simon Josefsson
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

#include <pskc/pskc.h>

#include <stdio.h>
#include <string.h>

const char *pskc_expect =
  "<?xml version=\"1.0\"?>\n"
  "<KeyContainer xmlns=\"urn:ietf:params:xml:ns:keyprov:pskc\" Version=\"42\" Id=\"MyID\"><KeyPackage><DeviceInfo><Manufacturer>iana.foo</Manufacturer><SerialNo>42</SerialNo><Model>model</Model><IssueNo>issueno</IssueNo><DeviceBinding>devbind</DeviceBinding><StartDate>1906-06-04T03:02:01Z</StartDate><ExpiryDate>1901-03-03T04:05:06Z</ExpiryDate><UserId>userid</UserId></DeviceInfo><CryptoModuleInfo><Id>cid</Id></CryptoModuleInfo><Key Id=\"keyid\" Algorithm=\"keyalg\"><Issuer>keyissuer</Issuer><AlgorithmParameters><Suite>keyalgparmsuite</Suite><ChallengeFormat Encoding=\"ALPHANUMERIC\" Min=\"4711\" Max=\"42\" CheckDigits=\"true\"/><ResponseFormat Encoding=\"BASE64\" Length=\"216\" CheckDigits=\"true\"/></AlgorithmParameters><KeyProfileId>profileid</KeyProfileId><KeyReference>keyref</KeyReference><FriendlyName>fname</FriendlyName><Data><Secret><PlainValue>Zm9v</PlainValue></Secret><Counter><PlainValue>4711</PlainValue></Counter><Time><PlainValue>12345</PlainValue></Time><TimeInterval><PlainValue>123456</PlainValue></TimeInterval><TimeDrift><PlainValue>321</PlainValue></TimeDrift></Data><UserId>keyuserid</UserId><Policy><StartDate>1906-06-04T03:02:01Z</StartDate><ExpiryDate>1901-03-03T04:05:06Z</ExpiryDate><PINPolicy PINKeyId=\"pinkeyid\" PINUsageMode=\"Append\" MaxFailedAttempts=\"123\" MinLength=\"24\" MaxLength=\"42\" PINEncoding=\"BASE64\"/><KeyUsage>Verify</KeyUsage><NumberOfTransactions>17</NumberOfTransactions></Policy></Key></KeyPackage></KeyContainer>\n";

void
my_log (const char *msg)
{
  puts (msg);
}

int
precheck (pskc_t * pskc)
{
  pskc_key_t *keyp;

  keyp = pskc_get_keypackage (pskc, 0);
  if (keyp == NULL)
    {
      printf ("pskc_get_keypackage\n");
      return 1;
    }

  {
    int p;
    pskc_valueformat v = pskc_get_key_algparm_chall_encoding (keyp, &p);
    if (p != 0)
      {
	printf ("pskc_get_key_algparm_chall_encoding pre\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_algparm_chall_min (keyp, &p);
    if (p != 0)
      {
	printf ("pskc_get_key_algparm_chall_min pre\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_algparm_chall_max (keyp, &p);
    if (p != 0)
      {
	printf ("pskc_get_key_algparm_chall_max pre\n");
	return 1;
      }
  }

  {
    int p;
    int b = pskc_get_key_algparm_chall_checkdigits (keyp, &p);
    if (p != 0)
      {
	printf ("pskc_get_key_algparm_chall_checkdigits pre\n");
	return 1;
      }
  }

  return 0;
}

int
check (pskc_t * pskc)
{
  pskc_key_t *keyp;

  {
    const char *v = pskc_get_version (pskc);
    if (strcmp (v, "42") != 0)
      {
	printf ("pskc_get_version\n");
	return 1;
      }
  }

  {
    const char *i = pskc_get_id (pskc);
    if (strcmp (i, "MyID") != 0)
      {
	printf ("pskc_get_id\n");
	return 1;
      }
  }

  keyp = pskc_get_keypackage (pskc, 0);
  if (keyp == NULL)
    {
      printf ("pskc_get_keypackage\n");
      return 1;
    }

  {
    const char *m = pskc_get_device_manufacturer (keyp);
    if (strcmp (m, "iana.foo") != 0)
      {
	printf ("pskc_get_device_manufacturer\n");
	return 1;
      }
  }

  {
    const char *s = pskc_get_device_serialno (keyp);
    if (strcmp (s, "42") != 0)
      {
	printf ("pskc_get_device_serialno\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_model (keyp);
    if (strcmp (m, "model") != 0)
      {
	printf ("pskc_get_device_model\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_issueno (keyp);
    if (strcmp (m, "issueno") != 0)
      {
	printf ("pskc_get_device_issueno\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_devicebinding (keyp);
    if (strcmp (m, "devbind") != 0)
      {
	printf ("pskc_get_device_devicebinding\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_device_startdate (keyp);
    if (t == NULL ||
	t->tm_sec != 1 || t->tm_min != 2 || t->tm_hour != 3 ||
	t->tm_mday != 4 || t->tm_mon != 5 || t->tm_year != 6)
      {
	printf ("pskc_get_device_startdate\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_device_expirydate (keyp);
    if (t == NULL ||
	t->tm_sec != 6 || t->tm_min != 5 || t->tm_hour != 4 ||
	t->tm_mday != 3 || t->tm_mon != 2 || t->tm_year != 1)
      {
	printf ("pskc_get_device_expirydate\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_device_userid (keyp);
    if (strcmp (m, "userid") != 0)
      {
	printf ("pskc_get_device_userid\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_cryptomodule_id (keyp);
    if (strcmp (m, "cid") != 0)
      {
	printf ("pskc_get_cryptomodule_id\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_id (keyp);
    if (strcmp (m, "keyid") != 0)
      {
	printf ("pskc_get_key_id\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_algorithm (keyp);
    if (strcmp (m, "keyalg") != 0)
      {
	printf ("pskc_get_key_algorithm\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_issuer (keyp);
    if (strcmp (m, "keyissuer") != 0)
      {
	printf ("pskc_get_key_issuer\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_algparm_suite (keyp);
    if (strcmp (m, "keyalgparmsuite") != 0)
      {
	printf ("pskc_get_key_algparm_suite\n");
	return 1;
      }
  }

  {
    int p;
    pskc_valueformat v = pskc_get_key_algparm_chall_encoding (keyp, &p);
    pskc_valueformat v2 = pskc_get_key_algparm_chall_encoding (keyp, NULL);
    if (p != 1 || v != v2 || v != PSKC_VALUEFORMAT_ALPHANUMERIC)
      {
	printf ("pskc_get_key_algparm_chall_encoding\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_algparm_chall_min (keyp, &p);
    uint32_t m = pskc_get_key_algparm_chall_min (keyp, NULL);
    if (p != 1 || l != m || l != 4711)
      {
	printf ("pskc_get_key_algparm_chall_min\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_algparm_chall_max (keyp, &p);
    uint32_t m = pskc_get_key_algparm_chall_max (keyp, NULL);
    if (p != 1 || l != m || l != 42)
      {
	printf ("pskc_get_key_algparm_chall_max\n");
	return 1;
      }
  }

  {
    int p;
    int b = pskc_get_key_algparm_chall_checkdigits (keyp, &p);
    int c = pskc_get_key_algparm_chall_checkdigits (keyp, NULL);
    if (p != 1 || b != c || b != 1)
      {
	printf ("pskc_get_key_algparm_chall_checkdigits\n");
	return 1;
      }
  }

  {
    int p;
    pskc_valueformat v = pskc_get_key_algparm_resp_encoding (keyp, &p);
    pskc_valueformat v2 = pskc_get_key_algparm_resp_encoding (keyp, NULL);
    if (p != 1 || v != v2 || v != PSKC_VALUEFORMAT_BASE64)
      {
	printf ("pskc_get_key_algparm_resp_encoding\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_algparm_resp_length (keyp, &p);
    uint32_t m = pskc_get_key_algparm_resp_length (keyp, NULL);
    if (p != 1 || l != m || l != 216)
      {
	printf ("pskc_get_key_algparm_resp_length\n");
	return 1;
      }
  }

  {
    int p;
    int b = pskc_get_key_algparm_resp_checkdigits (keyp, &p);
    int c = pskc_get_key_algparm_resp_checkdigits (keyp, NULL);
    if (p != 1 || b != c || b != 1)
      {
	printf ("pskc_get_key_algparm_resp_checkdigits\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_profileid (keyp);
    if (strcmp (m, "profileid") != 0)
      {
	printf ("pskc_get_key_profileid\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_reference (keyp);
    if (strcmp (m, "keyref") != 0)
      {
	printf ("pskc_get_key_reference\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_friendlyname (keyp);
    if (strcmp (m, "fname") != 0)
      {
	printf ("pskc_get_key_friendlyname\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_userid (keyp);
    if (strcmp (m, "keyuserid") != 0)
      {
	printf ("pskc_get_key_userid\n");
	return 1;
      }
  }

  {
    const char *s = pskc_get_key_data_b64secret (keyp);
    if (s == NULL || strcmp (s, "Zm9v") != 0)
      {
	printf ("pskc_get_key_data_b64secret\n");
	return 1;
      }
  }

  {
    const char *s = pskc_get_key_data_secret (keyp, NULL);
    if (s == NULL)
      {
	printf ("pskc_get_key_data_secret NULL\n");
	return 1;
      }
  }

  {
    size_t l;
    const char *s = pskc_get_key_data_secret (keyp, &l);
    if (s == NULL || memcmp (s, "foo", l) != 0)
      {
	printf ("pskc_get_key_data_secret\n");
	return 1;
      }
  }

  {
    int p;
    uint64_t l = pskc_get_key_data_counter (keyp, &p);
    uint64_t m = pskc_get_key_data_counter (keyp, NULL);
    if (p != 1 || l != m || l != 4711)
      {
	printf ("pskc_get_key_data_counter\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_data_time (keyp, &p);
    uint32_t m = pskc_get_key_data_time (keyp, NULL);
    if (p != 1 || l != m || l != 12345)
      {
	printf ("pskc_get_key_data_time\n");
	return 1;
      }
  }
  {
    int p;
    uint32_t l = pskc_get_key_data_timeinterval (keyp, &p);
    uint32_t m = pskc_get_key_data_timeinterval (keyp, NULL);
    if (p != 1 || l != m || l != 123456)
      {
	printf ("pskc_get_key_data_timeinterval\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_data_timedrift (keyp, &p);
    uint32_t m = pskc_get_key_data_timedrift (keyp, NULL);
    if (p != 1 || l != m || l != 321)
      {
	printf ("pskc_get_key_data_timedrift\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_key_policy_startdate (keyp);
    if (t == NULL ||
	t->tm_sec != 1 || t->tm_min != 2 || t->tm_hour != 3 ||
	t->tm_mday != 4 || t->tm_mon != 5 || t->tm_year != 6)
      {
	printf ("pskc_get_key_policy_startdate\n");
	return 1;
      }
  }

  {
    const struct tm *t = pskc_get_key_policy_expirydate (keyp);
    if (t == NULL ||
	t->tm_sec != 6 || t->tm_min != 5 || t->tm_hour != 4 ||
	t->tm_mday != 3 || t->tm_mon != 2 || t->tm_year != 1)
      {
	printf ("pskc_get_key_policy_expirydate\n");
	return 1;
      }
  }

  {
    const char *m = pskc_get_key_policy_pinkeyid (keyp);
    if (strcmp (m, "pinkeyid") != 0)
      {
	printf ("pskc_get_key_policy_pinkeyid\n");
	return 1;
      }
  }

  {
    int p;
    pskc_pinusagemode u = pskc_get_key_policy_pinusagemode (keyp, &p);
    pskc_pinusagemode v = pskc_get_key_policy_pinusagemode (keyp, NULL);
    if (p != 1 || u != v || u != PSKC_PINUSAGEMODE_APPEND)
      {
	printf ("pskc_get_key_policy_pinusagemode\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_policy_pinmaxfailedattempts (keyp, &p);
    uint32_t m = pskc_get_key_policy_pinmaxfailedattempts (keyp, NULL);
    if (p != 1 || l != m || l != 123)
      {
	printf ("pskc_get_key_policy_pinmaxfailedattempts\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_policy_pinminlength (keyp, &p);
    uint32_t m = pskc_get_key_policy_pinminlength (keyp, NULL);
    if (p != 1 || l != m || l != 24)
      {
	printf ("pskc_get_key_policy_pinminlength\n");
	return 1;
      }
  }

  {
    int p;
    uint32_t l = pskc_get_key_policy_pinmaxlength (keyp, &p);
    uint32_t m = pskc_get_key_policy_pinmaxlength (keyp, NULL);
    if (p != 1 || l != m || l != 42)
      {
	printf ("pskc_get_key_policy_pinmaxlength\n");
	return 1;
      }
  }

  {
    int p;
    pskc_valueformat v = pskc_get_key_policy_pinencoding (keyp, &p);
    pskc_valueformat w = pskc_get_key_policy_pinencoding (keyp, NULL);
    if (p != 1 || v != w || v != PSKC_VALUEFORMAT_BASE64)
      {
	printf ("pskc_get_key_policy_pinencoding\n");
	return 1;
      }
  }

  {
    int p;
    int v = pskc_get_key_policy_keyusages (keyp, &p);
    int w = pskc_get_key_policy_keyusages (keyp, NULL);
    if (p != 1 || v != w || v != PSKC_KEYUSAGE_VERIFY)
      {
	printf ("pskc_get_key_policy_keyusages\n");
	return 1;
      }
  }

  {
    int p;
    uint64_t l = pskc_get_key_policy_numberoftransactions (keyp, &p);
    uint64_t m = pskc_get_key_policy_numberoftransactions (keyp, NULL);
    if (p != 1 || l != m || l != 17)
      {
	printf ("pskc_get_key_policy_numberoftransactions\n");
	return 1;
      }
  }

  return 0;
}

int
main (void)
{
  pskc_t *pskc;
  pskc_key_t *keyp;
  struct tm startdate = { 1, 2, 3, 4, 5, 6 };
  struct tm expirydate = { 6, 5, 4, 3, 2, 1 };
  char *out;
  size_t len;
  int rc;

  rc = pskc_global_init ();
  if (rc != PSKC_OK)
    {
      printf ("pskc_global_init: %d\n", rc);
      return 1;
    }

  pskc_global_log (my_log);

  rc = pskc_init (&pskc);
  if (rc != PSKC_OK)
    {
      printf ("pskc_init: %d\n", rc);
      return 1;
    }

  pskc_set_version (pskc, "42");
  pskc_set_id (pskc, "MyID");

  rc = pskc_add_keypackage (pskc, &keyp);
  if (rc != PSKC_OK)
    {
      printf ("pskc_add_keypackage: %d\n", rc);
      return 1;
    }

  rc = precheck (pskc);
  if (rc != 0)
    return rc;

  pskc_set_device_manufacturer (keyp, "iana.foo");
  pskc_set_device_serialno (keyp, "42");
  pskc_set_device_model (keyp, "model");
  pskc_set_device_issueno (keyp, "issueno");
  pskc_set_device_devicebinding (keyp, "devbind");
  pskc_set_device_startdate (keyp, &startdate);
  pskc_set_device_expirydate (keyp, &expirydate);
  pskc_set_device_userid (keyp, "userid");
  pskc_set_cryptomodule_id (keyp, "cid");
  pskc_set_key_id (keyp, "keyid");
  pskc_set_key_algorithm (keyp, "keyalg");
  pskc_set_key_issuer (keyp, "keyissuer");
  pskc_set_key_algparm_suite (keyp, "keyalgparmsuite");
  pskc_set_key_algparm_chall_encoding (keyp, PSKC_VALUEFORMAT_ALPHANUMERIC);
  pskc_set_key_algparm_chall_min (keyp, 4711);
  pskc_set_key_algparm_chall_max (keyp, 42);
  pskc_set_key_algparm_chall_checkdigits (keyp, 1);
  pskc_set_key_algparm_resp_encoding (keyp, PSKC_VALUEFORMAT_BASE64);
  pskc_set_key_algparm_resp_length (keyp, 216);
  pskc_set_key_algparm_resp_checkdigits (keyp, 1);
  pskc_set_key_profileid (keyp, "profileid");
  pskc_set_key_reference (keyp, "keyref");
  pskc_set_key_friendlyname (keyp, "fname");
  pskc_set_key_userid (keyp, "keyuserid");
  pskc_set_key_data_counter (keyp, 4711);
  pskc_set_key_data_time (keyp, 12345);
  pskc_set_key_data_timeinterval (keyp, 123456);
  pskc_set_key_data_timedrift (keyp, 321);
  pskc_set_key_policy_startdate (keyp, &startdate);
  pskc_set_key_policy_expirydate (keyp, &expirydate);
  pskc_set_key_policy_pinkeyid (keyp, "pinkeyid");
  pskc_set_key_policy_pinusagemode (keyp, PSKC_PINUSAGEMODE_APPEND);
  pskc_set_key_policy_pinmaxfailedattempts (keyp, 123);
  pskc_set_key_policy_pinminlength (keyp, 24);
  pskc_set_key_policy_pinmaxlength (keyp, 42);
  pskc_set_key_policy_pinencoding (keyp, PSKC_VALUEFORMAT_BASE64);
  pskc_set_key_policy_keyusages (keyp, PSKC_KEYUSAGE_VERIFY);
  pskc_set_key_policy_numberoftransactions (keyp, 17);

  rc = pskc_set_key_data_secret (keyp, "foo", 3);
  if (rc != PSKC_OK)
    {
      printf ("pskc_set_key_data_secret: %d\n", rc);
      return 1;
    }

  rc = pskc_set_key_data_b64secret (keyp, "Zm9v");
  if (rc != PSKC_OK)
    {
      printf ("pskc_set_key_data_b64secret: %d\n", rc);
      return 1;
    }

  rc = pskc_set_key_data_b64secret (keyp, "nix");
  if (rc != PSKC_BASE64_ERROR)
    {
      printf ("pskc_set_key_data_b64secret: %d\n", rc);
      return 1;
    }

  rc = check (pskc);
  if (rc != 0)
    return rc;

  rc = pskc_build_xml (pskc, &out, &len);
  if (rc != PSKC_OK)
    {
      printf ("pskc_build_xml: %d\n", rc);
      return 1;
    }

  pskc_done (pskc);

  if (len != strlen (pskc_expect) || memcmp (pskc_expect, out, len) != 0)
    {
      printf ("generated data mismatch.  expected:\n%s\ngot:\n%.*s\n",
	      pskc_expect, (int) len, out);
      return 1;
    }

  rc = pskc_init (&pskc);
  if (rc != PSKC_OK)
    {
      printf ("pskc_init: %d\n", rc);
      return 1;
    }

  rc = pskc_parse_from_memory (pskc, len, out);
  if (rc != PSKC_OK)
    {
      printf ("pskc_parse_from_memory: %d\n", rc);
      return 1;
    }

  pskc_free (out);

  rc = check (pskc);
  if (rc != 0)
    return rc;

  pskc_done (pskc);

  pskc_global_done ();

  return 0;
}
