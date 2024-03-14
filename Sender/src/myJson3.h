#include <Arduino.h>


//const char* myJson1 =  "{'user_id':'1','name':'Amy', 'company':'ACME'}" ;

const char* myJson3 = {
    "1": {
        "user_id": "1",
        "name": "Amy",
        "mac_id": "xx01:xx01:xx01:xx01:xx01:xx01",
        "company": "Ace"},
    "2": {
        "user_id": "2",
        "name": "Bob",
        "mac_id": "xx02:xx02:xx02:xx02:xx02:xx02",
        "company": "Beta"},
	
     "3": {
        "user_id": "3",
        "name": "Charlie",
        "mac_id": "xx03:xx03:xx03:xx03:xx03:xx03",
        "company": "Capa"}
} ;


/*  ORIGIONAL *** BELOW
const char* myJson3 = {
    "1": {
        "user_id": "1",
        "name": "Amy",
        "mac_id": "xx01:xx01:xx01:xx01:xx01:xx01",
        "company": "Ace"},
    "2": {
        "user_id": "2",
        "name": "Bob",
        "mac_id": "xx02:xx02:xx02:xx02:xx02:xx02",
        "company": "Beta"},
	
     "3": {
        "user_id": "3",
        "name": "Charlie",
        "mac_id": "xx03:xx03:xx03:xx03:xx03:xx03",
        "company": "Capa"}
} ;


*/