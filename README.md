# CAC
Video Compression Algorithm

CORE IDEA

Till now video compression algorithms have evolved around compression based on object movements from frame to frame and intra frame compression.

The algorithm I am trying to invent is based on SRC. SRC stands for Set Redundancy Compression.

I am trying to achieve compression with MMPM+huffman. 
MMPM is an improvement Min Max Predictive (MMP).

SRC helps compress similar frames very effectively. Long enough video such as a movie is a set of similar sets of images where each set (a scene of a movie) contains similar images.

CAC attempts to exploit above mentioned property by compressing each set individually.


DIFFICULTY

Historgram generated to construct the huffman tree for a compressed frame still has all of 256 values possible after applying MMPM and hence it generates large codes for less probable values and nullifies all the bits saved from high probable values.

I am using a technique to calculate the difference between two numbers (Suggested by kosmas karadimitriou) such as same difference could be encoded with different numbers to avoid negative differences.

ETC

It might be little confusing at this point. I will post more descritption with code exmaples ASAP.

contribution from people working in the field of video compressio is welcomed. Currenlty, I am looking for the possibility of MMPM+RAR.

REFERENCES:

MIN-MAX COMPRESSION METHODS FOR MEDICAL IMAGE DATABASES by Kosmas karadimitriou and John M. Tyler
A COMPARISON OF SET REDUNDANCY COMPRESSION TECHNIQUES by Samy Ait-Audia and Abdelhalim Gabis
