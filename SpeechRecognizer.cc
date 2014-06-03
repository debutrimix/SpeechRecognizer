#include <node.h>
#include <pocketsphinx.h>
#include <stdio.h>
#include <v8.h>
#include <stdlib.h>

#define ARGS_EX "Invalid number of arguments,correct usage is:" \
                "SpeechRecognizer.process(array of samples" \
                ",hmm directory as string," \
                "lm path as string, dictionary path string," \
                "[jsgf (grammars file ) path as string]," \
                "[sample rate as integer default 16000hz])"


#define BUILDING_NODE_EXTENSION
#include <node.h>

using namespace v8;

Handle<Value> process(const Arguments& args) {

  HandleScope scope;
  ps_decoder_t *ps;
  cmd_ln_t *config;
  char const *hyp, *uttid;
  int rv;
  int32 score;
  char *hmm=NULL,*lm=NULL,*dict=NULL,*jsgf=NULL;
  char *srate="16000";
  int use_jsgf=0;
    
  if (args.Length() < 4 ){
    ThrowException(Exception::TypeError(String::New( ARGS_EX )));
    return scope.Close(Undefined());
  }
  
  if (!args[0]->IsArray())
  {
     ThrowException
     (
         Exception::TypeError
         (
           String::New
           (
             "Samples must be an array" 
           )
         )
      );
    return scope.Close(Undefined());

  }
  if (!args[1]->IsString() || !args[2]->IsString() ||
      !args[3]->IsString() )
  {
     ThrowException
     (
         Exception::TypeError
         (
           String::New
           (
             "hmm path must be string" 
           )
         )
      );
    return scope.Close(Undefined());

  }

  String::Utf8Value _hmm(args[1]);
  hmm = (char*) *_hmm;

  String::Utf8Value _lm(args[2]);
  lm= (char*) *_lm;
  
  String::Utf8Value _dict(args[3]);
  dict= (char*) *_dict;
  
  String::Utf8Value _jsgf(args[4]);
  jsgf =(char*) *_jsgf;

  if(strcmp(jsgf,"undefined"))
    use_jsgf=1;

  String::Utf8Value _srate(args[5]);
  srate =(char*) *_srate;
  if(!strcmp(srate,"undefined"))
    srate="16000";

  if(use_jsgf){
    config = cmd_ln_init(NULL,ps_args(), TRUE,
        "-hmm", hmm,
        "-lm", lm,
        "-dict",dict,
        "-samprate",srate,
        "-jsgf", jsgf,
        NULL);
  }
  else 
      config = cmd_ln_init(NULL,ps_args(), TRUE,
        "-hmm", hmm,
        "-lm", lm,
        "-dict",dict,
        "-samprate",srate,
        NULL);
        
  if (config == NULL)
    return scope.Close(Null());
  
  ps = ps_init(config);
  
  if (ps == NULL)
    return scope.Close(Null());
 

   v8::Handle<v8::Array> ar = v8::Handle<v8::Array>::Cast(args[0]);
  int16 *buf = (int16*)malloc( sizeof (int16) * ar->Length()); 
  
  for(int i=0; i != ar->Length(); i++){
     Local<Value> el = ar->Get(i);
     buf[i]=(el->ToInt32())->Value();
  }
  
  rv=ps_start_utt(ps,NULL); 
  ps_process_raw(ps,buf,ar->Length(),0,1);
  hyp = ps_get_hyp(ps, &score, &uttid);
  rv = ps_end_utt(ps);
  
  Local<String> str;
  str=String::New(hyp);  
  free(buf);
  ps_free(ps);

  return scope.Close(str);

}

void Init(Handle<Object> exports) {
  exports->Set(String::NewSymbol("process"),
               FunctionTemplate::New(process)->GetFunction()
              );
}

NODE_MODULE(SpeechRecognizer, Init)
