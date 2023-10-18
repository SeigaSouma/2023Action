xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 48;
 15.34570;13.53249;11.12991;,
 7.52821;27.96567;6.58127;,
 -3.16930;32.99165;10.02985;,
 -3.16930;21.14360;17.01955;,
 0.16648;-0.16901;21.94515;,
 18.26990;-0.16901;10.99275;,
 -11.28063;30.71545;10.02985;,
 -14.04123;21.14360;13.82789;,
 -17.78081;-0.16901;15.75508;,
 9.30947;17.66262;-18.65961;,
 15.71883;-0.16901;-17.45443;,
 0.16648;-0.16901;-24.67836;,
 0.16648;17.93442;-22.23740;,
 0.16648;31.03812;-13.25567;,
 7.49342;28.46389;-11.23549;,
 -18.65322;-0.16901;-17.45443;,
 -16.31740;17.93442;-14.92395;,
 -13.97740;28.95279;-10.39097;,
 18.26990;17.93442;-3.23086;,
 15.34570;13.53249;11.12991;,
 18.26990;-0.16901;10.99275;,
 21.26551;-0.16901;-3.23086;,
 15.71883;-0.16901;-17.45443;,
 9.30947;17.66262;-18.65961;,
 7.49342;28.46389;-11.23549;,
 11.27560;27.66361;-3.01823;,
 7.52821;27.96567;6.58127;,
 21.26551;-0.16901;-3.23086;,
 0.16648;-0.16901;0.64900;,
 0.16648;-0.16901;-24.67836;,
 15.71883;-0.16901;-17.45443;,
 11.27560;27.66361;-3.01823;,
 7.49342;28.46389;-11.23549;,
 0.16648;31.03812;-13.25567;,
 0.16648;34.40411;0.64900;,
 -17.78081;-0.16901;15.75508;,
 -25.24867;-0.16901;0.64900;,
 -18.65322;-0.16901;-17.45443;,
 -13.97740;28.95279;-10.39097;,
 -14.20029;31.61870;0.64900;,
 -11.28063;30.71545;10.02985;,
 -25.24867;-0.16901;0.64900;,
 -14.04123;21.14360;13.82789;,
 -19.71536;18.12741;0.16203;,
 -16.31740;17.93442;-14.92395;,
 -18.65322;-0.16901;-17.45443;,
 -14.20029;31.61870;0.64900;,
 -13.97740;28.95279;-10.39097;;
 
 24;
 4;0,1,2,3;,
 4;0,3,4,5;,
 4;3,2,6,7;,
 4;3,7,8,4;,
 4;9,10,11,12;,
 4;9,12,13,14;,
 4;12,11,15,16;,
 4;12,16,17,13;,
 4;18,19,20,21;,
 4;18,21,22,23;,
 4;18,23,24,25;,
 4;18,25,26,19;,
 4;27,20,4,28;,
 4;27,28,29,30;,
 4;31,32,33,34;,
 4;31,34,2,26;,
 4;28,4,35,36;,
 4;28,36,37,29;,
 4;34,33,38,39;,
 4;34,39,40,2;,
 4;41,35,42,43;,
 4;41,43,44,45;,
 4;46,47,44,43;,
 4;46,43,42,40;;
 
 MeshMaterialList {
  1;
  24;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.530196;0.530196;0.530196;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  26;
  -0.551630;0.688473;-0.470861;,
  -0.724905;0.204973;-0.657646;,
  0.553771;0.688660;-0.468065;,
  0.679024;0.169281;-0.714332;,
  0.614848;0.640317;0.460387;,
  0.774655;0.199931;0.599948;,
  -0.511559;0.696928;0.502592;,
  -0.654294;0.209796;0.726557;,
  0.158611;0.794717;0.585890;,
  0.083359;0.180626;0.980013;,
  0.082553;0.819586;-0.566977;,
  0.007832;0.138425;-0.990342;,
  0.718594;0.372293;0.587384;,
  0.109579;0.342059;0.933268;,
  0.646833;0.368831;-0.667511;,
  -0.635935;0.336396;0.694568;,
  0.016672;0.353950;-0.935116;,
  -0.717053;0.335151;-0.611154;,
  0.918445;0.388341;-0.075168;,
  0.972523;0.206657;-0.107200;,
  0.673847;0.738867;-0.002508;,
  0.000000;-1.000000;-0.000000;,
  0.145889;0.989243;-0.010740;,
  -0.648740;0.760962;0.008602;,
  -0.951750;0.300289;0.063242;,
  -0.968760;0.241431;0.056697;;
  24;
  4;12,4,8,13;,
  4;12,13,9,5;,
  4;13,8,6,15;,
  4;13,15,7,9;,
  4;14,3,11,16;,
  4;14,16,10,2;,
  4;16,11,1,17;,
  4;16,17,0,10;,
  4;18,12,5,19;,
  4;18,19,3,14;,
  4;18,14,2,20;,
  4;18,20,4,12;,
  4;21,21,21,21;,
  4;21,21,21,21;,
  4;20,2,10,22;,
  4;20,22,8,4;,
  4;21,21,21,21;,
  4;21,21,21,21;,
  4;22,10,0,23;,
  4;22,23,6,8;,
  4;25,7,15,24;,
  4;25,24,17,1;,
  4;23,0,17,24;,
  4;23,24,15,6;;
 }
 MeshTextureCoords {
  48;
  0.000000;0.500000;,
  0.000000;0.000000;,
  0.500000;0.000000;,
  0.500000;0.500000;,
  0.500000;1.000000;,
  0.000000;1.000000;,
  1.000000;0.000000;,
  1.000000;0.500000;,
  1.000000;1.000000;,
  1.000000;0.500000;,
  1.000000;1.000000;,
  0.500000;1.000000;,
  0.500000;0.500000;,
  0.500000;0.000000;,
  1.000000;0.000000;,
  0.000000;1.000000;,
  0.000000;0.500000;,
  0.000000;0.000000;,
  0.500000;0.500000;,
  1.000000;0.500000;,
  1.000000;1.000000;,
  0.500000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.500000;,
  0.000000;0.000000;,
  0.500000;0.000000;,
  1.000000;0.000000;,
  1.000000;0.500000;,
  0.500000;0.500000;,
  0.500000;0.000000;,
  1.000000;0.000000;,
  1.000000;0.500000;,
  1.000000;1.000000;,
  0.500000;1.000000;,
  0.500000;0.500000;,
  0.000000;1.000000;,
  0.000000;0.500000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  0.000000;0.500000;,
  0.000000;0.000000;,
  0.500000;1.000000;,
  0.000000;0.500000;,
  0.500000;0.500000;,
  1.000000;0.500000;,
  1.000000;1.000000;,
  0.500000;0.000000;,
  1.000000;0.000000;;
 }
}