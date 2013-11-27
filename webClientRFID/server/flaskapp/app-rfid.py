#!/usr/bin/env python

#import shelve
#from subprocess import check_output
import flask
#import operator
from flask import request
#from flask import url_for
from flask import abort
from os import environ

app = flask.Flask(__name__)
app.debug = True

#db = shelve.open("shorten.db")


###
# Home Resource:
# Only supports the GET method, returns a homepage represented as HTML
###
@app.route('/', methods=['GET'])
@app.route('/hello', methods=['GET'])
def home():
    """Builds a template based on a GET request, with some default
    arguements"""  
    # app.logger.debug(db) to print out
    rfid = request.args.get('rfid')
    if rfid:
    	return lookupAudio(rfid)
    else:
    	return "no rfid"

@app.route('/error', methods=['GET'])
def error():
    abort(404)
    
@app.errorhandler(404)
def page_not_found(e):
	"""Handles all requests that the server can't handle"""
	return flask.render_template("404.html", page=e)

def lookupAudio(rfid): 
	if rfid == "6A003E4B243B":
		return "C_SOUND.AFM"
	elif rfid == "6A003E1E3C76":
		return "A_SOUND.AFM"
	elif rfid == "6A003E503C38":
		return "T_SOUND.AFM"
	else: 
		return "NOT FOUND!"

if __name__ == "__main__":
    app.run(port=int(environ['FLASK_PORT']))
    #app.run(port=8080)
