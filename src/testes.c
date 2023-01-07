#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "json.h"
#include "operations.h"
#include "testes.h"
#include "validation.h"
#include "client.h"
#include <unistd.h>
#include <math.h>
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */


int main(){
    tests_json();
    tests_client();
    tests_operations();
    tests_validation();
    end_test();
    return 0;
}


void tests_json(){
    tests_remove_last_newline();
    tests_get_code();
    tests_get_calcul();
    tests_set_code();
    tests_set_message();
    tests_set_name();
    tests_set_calcul();
    tests_create_error_message();
    tests_create_ok_message();
    tests_create_ok_message_int();
    tests_prepare_message();
    tests_add_element();
    tests_add_first_element();
    tests_add_number_element();
}
void tests_remove_last_newline(){
    char tmp[] = "message\n";
    remove_last_newline(tmp);
    ASSERT_EQ_STR("message", tmp);
}

void tests_get_code(){
    char tmp[] = "{\"code\":\"avg\",\"values\":[1, 2, 3, 4, 5]}";
    ASSERT_EQ_STR("avg", get_code(tmp));
    char tmp2[] = "{\"code\":\"ect\",\"values\":[1, 2, 3, 4, 5]}";
    ASSERT_EQ_STR("ect", get_code(tmp2));
    char tmp3[] = "{\"code\":\"nam\",\"values\":[1, 2, 3, 4, 5]}";
    ASSERT_EQ_STR("nam", get_code(tmp3));
}

void tests_get_calcul(){
    struct Calc to_calc;
    char tmp[] = "{\"code\":\"sum\",\"values\":[\"+\",2.2,3.3]}";
    ASSERT_EQ_INT(0, get_calcul(tmp, &to_calc));
    ASSERT_EQ_CHAR('+', to_calc.operation);
    ASSERT_EQ_FLOAT(2.2, to_calc.nums[0]);
    ASSERT_EQ_FLOAT(3.3, to_calc.nums[1]);
    char tmp2[] = "{\"code\":\"sum\",\"values\":[\"+\"]}";
    ASSERT_EQ_INT(1, get_calcul(tmp2, &to_calc));
}

void tests_prepare_message(){
    char message[1024];
    prepare_message(message, CODE_AVG);
    ASSERT_EQ_STR("{\"code\":\"avg\",\"values\":[", message);
}

void tests_set_code(){
    char tmp[1024] = {0};
    strcat(tmp, FIRST_JSON_PART);
    set_code(CODE_AVG, tmp);
    ASSERT_EQ_STR("{\"code\":\"avg\",\"values\":[", tmp);
}

void tests_set_message(){
    char tmp[1024] = {0};
    set_message("message", tmp);
    ASSERT_EQ_STR("{\"code\":\"msg\",\"values\":[\"message\"]}", tmp);
}

void tests_set_name(){
    char tmp[1024] = {0};
    char name[] = "name";
    set_name(name, tmp);
    ASSERT_EQ_STR("{\"code\":\"nam\",\"values\":[\"name\"]}", tmp);
}

void tests_set_calcul(){
    char tmp[1024] = {0};
    char calc[] = "+ 1.2 2.3";
    prepare_message(tmp, CODE_CAL);
    set_calcul(calc, tmp);
    ASSERT_EQ_STR("{\"code\":\"cal\",\"values\":[\"+\",1.20,2.30]}", tmp);
}

void tests_create_error_message(){
    char tmp[1024] = {0};
    create_error_message(tmp, "error");
    ASSERT_EQ_STR("{\"code\":\"err\",\"values\":[\"error: error\"]}", tmp);
}

void tests_create_ok_message(){
    char tmp[1024] = {0};
    create_ok_message(tmp, "ok");
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[\"ok\"]}", tmp);
}

void tests_create_ok_message_int(){
    char tmp[1024] = {0};
    create_ok_message_int(tmp, "1337");
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[1337]}", tmp);
}

void tests_add_element(){
    char tmp[1024] = {0};
    prepare_message(tmp, CODE_COL);
    add_first_element(tmp, "1");
    add_element(tmp, "message");
    strcat(tmp, "]}");
    ASSERT_EQ_STR("{\"code\":\"col\",\"values\":[1,\"message\"]}", tmp);
}

void tests_add_first_element(){
    char tmp[1024] = {0};
    prepare_message(tmp, CODE_COL);
    add_first_element(tmp, "0");
    strcat(tmp, "]}");
    ASSERT_EQ_STR("{\"code\":\"col\",\"values\":[0]}", tmp);
}

void tests_add_number_element(){
    char tmp[1024] = {0};
    prepare_message(tmp, CODE_COL);
    strcat(tmp, "1");
    add_number_element(tmp, "1337");
    strcat(tmp, "]}");
    ASSERT_EQ_STR("{\"code\":\"col\",\"values\":[1,1337]}", tmp);
}

void tests_client(){
    test_envoie_recois_name();
}

void test_envoie_recois_name(){
    char message[1024] = {0};
    envoie_recois_name(message);
    char expected[1024] = {0};
    char hostname[1024] = {0};
    gethostname(hostname, 1024);
    set_name(hostname, expected);
    ASSERT_EQ_STR(expected, message);
}

void tests_operations(){
    test_calculator();
    test_calcul();
    test_mini();
    test_maxi();
    test_avg();
    test_ecart();
}
void test_calculator(){
    struct Calc c;
    c.operation = '+';
    c.nums[0] = 1.2;
    c.nums[1] = 2.3;
    float out = 0.0;
    ASSERT_EQ_FLOAT(0, calculator(&c, &out));
    ASSERT_EQ_FLOAT(3.5, out);
    c.operation = '-';
    ASSERT_EQ_FLOAT(0, calculator(&c, &out));
    ASSERT_EQ_FLOAT(round(-1.1), round(out));
    c.operation = '*';
    ASSERT_EQ_FLOAT(0, calculator(&c, &out));
    ASSERT_EQ_FLOAT(2.76, out);
    c.operation = '/';
    ASSERT_EQ_FLOAT(0, calculator(&c, &out));
    ASSERT_EQ_DOUBLE(round(0.521739 * 100), round(out * 100));
    c.operation = 'a';
    ASSERT_EQ_FLOAT(-3, calculator(&c, &out));
}

void test_calcul(){
    char tmp[1024] = "{\"code\":\"cal\",\"values\":[\"+\",2.2,3.3]}";
    ASSERT_EQ_INT(0, calcul(tmp));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[5.500000]}", tmp);
    char tmp2[1024] = "{\"code\":\"cal\",\"values\":[\"-\",3.3,2.2]}";
    ASSERT_EQ_INT(0, calcul(tmp2));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[1.100000]}", tmp2);
    char tmp3[1024] = "{\"code\":\"cal\",\"values\":[\"*\",2.2,3.3]}";
    ASSERT_EQ_INT(0, calcul(tmp3));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[7.260000]}", tmp3);
    char tmp4[1024] = "{\"code\":\"cal\",\"values\":[\"/\",3.3,2.2]}";
    ASSERT_EQ_INT(0, calcul(tmp4));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[1.500000]}", tmp4);
    char tmp5[1024] = "{\"code\":\"cal\",\"values\":[\"a\",3.3,2.2]}";
    ASSERT_EQ_INT(1, calcul(tmp5));
}

void test_mini(){
    char tmp[1024] = "{\"code\":\"min\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(0, mini(tmp));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[1]}", tmp);
    char tmp2[1024] = "{\"code\":\"min\",\"values\":[\"a\"]}";
    ASSERT_EQ_INT(0, mini(tmp2));
    ASSERT_EQ_STR("{\"code\":\"err\",\"values\":[\"error: couldn't parse min value\"]}", tmp2);
}

void test_maxi(){
    char tmp[1024] = "{\"code\":\"max\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(0, maxi(tmp));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[3]}", tmp);
    char tmp2[1024] = "{\"code\":\"max\",\"values\":[\"a\"]}";
    ASSERT_EQ_INT(0, maxi(tmp2));
    ASSERT_EQ_STR("{\"code\":\"err\",\"values\":[\"error: couldn't parse max value\"]}", tmp2);
}

void test_avg(){
    char tmp[1024] = "{\"code\":\"avg\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(0, avg(tmp));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[2.000000]}", tmp);
    char tmp2[1024] = "{\"code\":\"avg\",\"values\":[\"a\"]}";
    ASSERT_EQ_INT(1, avg(tmp2));
    ASSERT_EQ_STR("{\"code\":\"err\",\"values\":[\"error: couldn't parse avg value\"]}", tmp2);
}

void test_ecart(){
    char tmp[1024] = "{\"code\":\"ecr\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(0, ecart(tmp));
    ASSERT_EQ_STR("{\"code\":\"okk\",\"values\":[0.816497]}", tmp);
    char tmp2[1024] = "{\"code\":\"ect\",\"values\":[\"a\"]}";
    ASSERT_EQ_INT(0, ecart(tmp2));
    ASSERT_EQ_STR("{\"code\":\"err\",\"values\":[\"error: couldn't parse ect value\"]}", tmp2);
}

void tests_validation(){
    test_json_validator();
}

void test_json_validator(){
    // good ones
    char tmp[1024] = "{\"code\":\"cal\",\"values\":[\"+\",2.2,3.3]}";
    ASSERT_EQ_INT(1, json_validator(tmp));
    char tmp2[1024] = "{\"code\":\"msg\",\"values\":[\"message\"]}";
    ASSERT_EQ_INT(1, json_validator(tmp2));
    char tmp3[1024] = "{\"code\":\"min\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(1, json_validator(tmp3));
    char tmp4[1024] = "{\"code\":\"max\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(1, json_validator(tmp4));
    char tmp5[1024] = "{\"code\":\"avg\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(1, json_validator(tmp5));
    char tmp6[1024] = "{\"code\":\"col\",\"values\":[10, \"red\", \"green\", \"blue\", \"yellow\", \"pink\", \"purple\", \"brown\", \"black\", \"white\", \"grey\"]}";
    ASSERT_EQ_INT(1, json_validator(tmp6));
    char tmp7[1024] = "{\"code\":\"ecr\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(1, json_validator(tmp7));
    char tmp8[1024] = "{\"code\":\"err\",\"values\":[\"error: error\"]}";
    ASSERT_EQ_INT(1, json_validator(tmp8));
    char tmp9[1024] = "{\"code\":\"okk\",\"values\":[\"ok: ok\"]}";
    ASSERT_EQ_INT(1, json_validator(tmp9));
    char tmp10[1024] = "{\"code\":\"nam\",\"values\":[\"nom cool\"]}";
    ASSERT_EQ_INT(1, json_validator(tmp10));
    // bad ones
    char tmp11[1024] = "{\"code\":\"cal\",\"values\":[\"+,2.2,3.3,4.4]}";
    ASSERT_EQ_INT(0, json_validator(tmp11));
    char tmp12[1024] = "{\"code\":\"msg\",\"values\":[\"message\"]";
    ASSERT_EQ_INT(0, json_validator(tmp12));
    char tmp13[1024] = "{\"code\":min\",\"values\":[3,1,2,3]";
    ASSERT_EQ_INT(0, json_validator(tmp13));
    char tmp14[1024] = "\"code\":\"max\",\"values\":[3,1,2,3]}";
    ASSERT_EQ_INT(0, json_validator(tmp14));
}