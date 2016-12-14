#include <yaml-cpp/yaml.h>
#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <twitter.h>
#include <verbly.h>
#include <chrono>
#include <thread>

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::cout << "usage: wordplay [configfile]" << std::endl;
    return -1;
  }

  std::string configfile(argv[1]);
  YAML::Node config = YAML::LoadFile(configfile);
    
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());
  
  twitter::client client(auth);
  
  verbly::data database(config["verbly_datafile"].as<std::string>());
  
  for (;;)
  {
    // Generate the most amazing jokes you've ever heard
    auto adjq = database.adjectives().has_rhyming_noun().has_synonyms().random().limit(1).run();
    if (adjq.empty())
    {
      continue;
    }
    
    verbly::adjective rhmadj = adjq.front();
    
    auto nounq = database.nouns().rhymes_with(rhmadj).is_hyponym().random().limit(1).run();
    if (nounq.empty())
    {
      continue;
    }
    
    verbly::noun rhmnoun = nounq.front();
    
    auto hypq = database.nouns().hypernym_of(rhmnoun).random().limit(1).run();
    if (hypq.empty())
    {
      continue;
    }
    
    verbly::noun hyp = hypq.front();
    
    auto synq = database.adjectives().synonym_of(rhmadj).random().limit(1).run();
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
    
    try
    {
      client.updateStatus(result.str());
      
      std::cout << "Tweeted!" << std::endl;
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }

    std::cout << "Waiting..." << std::endl;
    
    std::this_thread::sleep_for(std::chrono::hours(1));
  }
}
