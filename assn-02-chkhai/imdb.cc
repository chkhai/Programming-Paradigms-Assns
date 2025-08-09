using namespace std;
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "imdb.h"

const char *const imdb::kActorFileName = "actordata";
const char *const imdb::kMovieFileName = "moviedata";

imdb::imdb(const string& directory)
{
  const string actorFileName = directory + "/" + kActorFileName;
  const string movieFileName = directory + "/" + kMovieFileName;
  
  actorFile = acquireFileMap(actorFileName, actorInfo);
  movieFile = acquireFileMap(movieFileName, movieInfo);
}

bool imdb::good() const
{
  return !( (actorInfo.fd == -1) || 
	    (movieInfo.fd == -1) ); 
}

// you should be implementing these two methods right here... 
bool imdb::getCredits(const string& player, vector<film>& films) const { 
  int n_actors = *(int*)actorFile;
  
  int leftIdx = 0;
  int rightIdx = n_actors-1;
  int midIdx;
  while(leftIdx <= rightIdx){
    midIdx = (leftIdx + rightIdx) / 2;
    int offset_of_player_name = *(int*)((char*)(actorFile) + 4 + midIdx * 4);
    string name = (char*)((char*)actorFile + offset_of_player_name);

    if(player == name){
      int offset_from_name_to_movies = player.length();
      if(player.length() % 2 == 0){
        offset_from_name_to_movies += 2;
      }else{
        offset_from_name_to_movies += 1;
      }

      short num_movies = *(short*)((char*)actorFile + offset_of_player_name + offset_from_name_to_movies);

      int padding = 2;
      if((offset_from_name_to_movies+2) % 4 != 0) padding +=2;

      for(int i = 0; i < num_movies; i ++){
        int curr_movie_idx = *(int*)((char*)actorFile + offset_of_player_name + offset_from_name_to_movies + padding + i*4);
        film f;
        string movie = (char*)((char*)movieFile + curr_movie_idx);
        int year = 1900 + *(char*)((char*)movieFile+ curr_movie_idx + movie.size() + 1); 
        f.title = movie;
        f.year = year;
        films.push_back(f);
      }
      return true;
    } else if(player < name){
      rightIdx = midIdx - 1;
    }else{
      leftIdx = midIdx + 1;
    }
  }
  return false;
}

bool imdb::getCast(const film& movie, vector<string>& players) const { 
  int n_movies = *(int*)movieFile;

  int leftIdx = 0;
  int rightIdx = n_movies-1;
  int midIdx;
  while(leftIdx <= rightIdx){
    midIdx = (leftIdx + rightIdx) / 2;
    int offset_of_movie_title = *(int*)((char*)movieFile + 4 + midIdx * 4);
    char* curr_ptr = (char*)movieFile + offset_of_movie_title; 
    string title(curr_ptr);
    int size = movie.title.size();
    curr_ptr += title.length()+1;

    int year = 1900 + *curr_ptr;
    curr_ptr++;

    film curr_film = {title, year};
    if(movie == curr_film){
      if(size % 2 != 0) curr_ptr++;
      short n_actors = *(short*)(curr_ptr);
      curr_ptr += 2;

      if(size % 4 == 2) curr_ptr+=2;
      else if((size+1) % 4 == 2) curr_ptr+=2;

      for(int i = 0; i < n_actors; i++){
        int curr_offset = *(int*)((char*)curr_ptr + 4*i);
        char* act = (char*)actorFile + curr_offset;
        string s(act);
        players.push_back(s);
      }
    return true;
    }else if(movie < curr_film){
      rightIdx = midIdx - 1;
    }else{
      leftIdx = midIdx + 1;
    }
  }
  return false; 
}

imdb::~imdb()
{
  releaseFileMap(actorInfo);
  releaseFileMap(movieInfo);
}

// ignore everything below... it's all UNIXy stuff in place to make a file look like
// an array of bytes in RAM.. 
const void *imdb::acquireFileMap(const string& fileName, struct fileInfo& info)
{
  struct stat stats;
  stat(fileName.c_str(), &stats);
  info.fileSize = stats.st_size;
  info.fd = open(fileName.c_str(), O_RDONLY);
  return info.fileMap = mmap(0, info.fileSize, PROT_READ, MAP_SHARED, info.fd, 0);
}

void imdb::releaseFileMap(struct fileInfo& info)
{
  if (info.fileMap != NULL) munmap((char *) info.fileMap, info.fileSize);
  if (info.fd != -1) close(info.fd);
}
