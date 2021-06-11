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

  verbly::database database(config["verbly_datafile"].as<std::string>());

  // Blacklist some stuff
  verbly::filter cleanFilter =
    !(verbly::word::usageDomains %= (verbly::notion::wnid == 106718862)) // ethnic slurs
    && !(verbly::notion::wnid == 110630093); // "spastic"

  verbly::filter nounFilter =
    cleanFilter
    && (verbly::notion::partOfSpeech == verbly::part_of_speech::noun)
    && (verbly::notion::hypernyms %= cleanFilter);

  verbly::query<verbly::word> adjectiveQuery = database.words(
    (verbly::notion::partOfSpeech == verbly::part_of_speech::adjective)
    && (verbly::pronunciation::rhymes %= nounFilter)
    && (verbly::word::synonyms));

  for (;;)
  {
    verbly::word adjective = adjectiveQuery.first();

    verbly::word noun = database.words(
      nounFilter
      && (verbly::pronunciation::rhymes %= adjective)).first();

    verbly::word hypernym = database.words(
      cleanFilter
      && (verbly::notion::hyponyms %= noun)).first();

    verbly::word synonym = database.words(
      (verbly::word::synonyms %= adjective)).first();

    verbly::token action = verbly::token::separator("\n", {
      verbly::token::punctuation("?", {
        "What do you call",
        verbly::token::indefiniteArticle(synonym),
        hypernym
      }),
      verbly::token::capitalize(verbly::token::casing::capitalize,
        verbly::token::punctuation("!", {
          verbly::token::indefiniteArticle(adjective),
          noun
        }))
    });

    std::string result = action.compile();
    std::cout << result << std::endl;

    try
    {
      client.updateStatus(result);

      std::cout << "Tweeted!" << std::endl;
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }

    std::cout << "Waiting..." << std::endl;

    std::this_thread::sleep_for(std::chrono::hours(1));

    std::cout << std::endl;
  }
}

