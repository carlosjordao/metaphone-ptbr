#!/usr/bin/python
# -*- coding: utf-8 -*-

import metaphoneptbr

orig = ['carlos', 'concerto', 'chapéu', 'assombro', 'carro']
resp = ['KRLS','KNSRT','XP','ASMBR','K2']

for i in range(len(orig)):
	print "String '"+orig[i]+"' 	should return '"+resp[i]+"': 	"+ metaphoneptbr.metaphoneptbr(orig[i])
