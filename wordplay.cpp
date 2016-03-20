#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <twitcurl.h>
#include <verbly.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  srand(time(NULL));
  
  YAML::Node config = YAML::LoadFile("config.yml");
    
  twitCurl twitter;
  twitter.getOAuth().setConsumerKey(config["consumer_key"].as<std::string>());
  twitter.getOAuth().setConsumerSecret(config["consumer_secret"].as<std::string>());
  twitter.getOAuth().setOAuthTokenKey(config["access_key"].as<std::string>());
  twitter.getOAuth().setOAuthTokenSecret(config["access_secret"].as<std::string>());
  
  verbly::data database("data.sqlite3");
  
  for (;;)
  {
    // Generate the most amazing jokes you've ever heard
    auto adjq = database.adjectives().has_pronunciation(true).has_synonyms(true).random(true).limit(1).run();
    if (adjq.empty())
    {
      continue;
    }
    
    verbly::adjective rhmadj = adjq.front();
    
    auto nounq = database.nouns().rhymes_with(rhmadj).not_derived_from(rhmadj).is_hyponym(true).random(true).limit(1).run();
    if (nounq.empty())
    {
      continue;
    }
    
    verbly::noun rhmnoun = nounq.front();
    
    auto hypq = database.nouns().hypernym_of(rhmnoun).random(true).limit(1).run();
    if (hypq.empty())
    {
      continue;
    }
    
    verbly::noun hyp = hypq.front();
    
    auto synq = database.adjectives().synonym_of(rhmadj).random(true).limit(1).run();
    if (synq.empty())
    {
      continue;
    }
    
    verbly::adjective syn = synq.front();
    
    std::stringstream result;
    if (syn.starts_with_vowel_sound())
    {
      result << "What do you call an " << syn.base_form() << " " << hyp.base_form() << "?" << std::endl;
    } else {
      result << "What do you call a " << syn.base_form() << " " << hyp.base_form() << "?" << std::endl;
    }
    
    if (rhmadj.starts_with_vowel_sound())
    {
      result << "An " << rhmadj.base_form() << " " << rhmnoun.base_form() << "!" << std::endl;
    } else {
      result << "A " << rhmadj.base_form() << " " << rhmnoun.base_form() << "!" << std::endl;
    }
    
    std::string replyMsg;
    if (twitter.statusUpdate(result.str()))
    {
      twitter.getLastWebResponse(replyMsg);
      std::cout << "Twitter message: " << replyMsg << std::endl;
    } else {
      twitter.getLastCurlError(replyMsg);
      std::cout << "Curl error: " << replyMsg << std::endl;
    }

    std::cout << "Waiting" << std::endl;
    
    sleep(60 * 60 * 3);
  }
}
