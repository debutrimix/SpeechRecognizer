{
  "targets": [ 
  {
   "target_name": "SpeechRecognizer",
   "sources": [ "SpeechRecognizer.cc" ],
   "cflags!":[
   ],
   "libraries":[
     "-lpocketsphinx","-lsphinxbase",
   ],
   "include_dirs":[
     "/usr/local/include/pocketsphinx",
     "/usr/local/include/sphinxbase/",
   ],
  }
  ]
}
