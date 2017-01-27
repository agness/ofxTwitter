//
// Copyright (c) 2009 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier:	MIT
//


#include "ofx/Twitter/StreamingClient.h"
#include "ofx/HTTP/HTTPUtils.h"


namespace ofx {
namespace Twitter {


StreamingRequestParameters::~StreamingRequestParameters()
{
}

void StreamingRequestParameters::setDelimited(Delimited delimited)
{
    _delimited = delimited;
}


void StreamingRequestParameters::setStallWarnings(bool stallWarnings)
{
    _stallWarnings = stallWarnings;
}


void StreamingRequestParameters::setFilterLevel(FilterLevel filterLevel)
{
    _filterLevel = filterLevel;
}

void StreamingRequestParameters::setLanguage(const std::string& language)
{
    setLanguages({ language });
}


void StreamingRequestParameters::setLanguages(const std::vector<std::string>& languages)
{
    _languages = languages;
}


Poco::Net::NameValueCollection StreamingRequestParameters::toNameValueCollection() const
{
    Poco::Net::NameValueCollection parameters;

    parameters.set("stall_warnings", _stallWarnings ? "true" : "false");

    if (_delimited == Delimited::LENGTH) parameters.set("delimited", "length");

    if (_filterLevel == FilterLevel::LOW) parameters.set("filter_level", "low");
    else if (_filterLevel == FilterLevel::MEDIUM) parameters.set("filter_level", "medium");
    else if (_filterLevel == FilterLevel::NONE) parameters.set("filter_level", "none");

    if (!_languages.empty()) parameters.set("language", HTTP::HTTPUtils::join(_languages, ","));

    return parameters;
}


BaseFilterStreamingRequestParameters::~BaseFilterStreamingRequestParameters()
{
}


void BaseFilterStreamingRequestParameters::setTrack(const std::string& track)
{
    setTracks({ track });
}

void BaseFilterStreamingRequestParameters::setTracks(const std::vector<std::string>& tracks)
{
    _tracks = tracks;
}

void BaseFilterStreamingRequestParameters::setLocation(const Geo::CoordinateBounds& location)
{
    setLocations({ location });
}


void BaseFilterStreamingRequestParameters::setLocations(const std::vector<Geo::CoordinateBounds>& locations)
{
    _locations = locations;
}



Poco::Net::NameValueCollection BaseFilterStreamingRequestParameters::toNameValueCollection() const
{
    Poco::Net::NameValueCollection parameters = StreamingRequestParameters::toNameValueCollection();

    if (!_tracks.empty()) parameters.set("track", HTTP::HTTPUtils::join(_tracks, ","));

    if (!_locations.empty())
    {
        std::vector<double> values;

        for (auto& bound: _locations)
        {
            values.push_back(bound.getSouthWest().getLongitude());
            values.push_back(bound.getSouthWest().getLatitude());
            values.push_back(bound.getNorthEast().getLongitude());
            values.push_back(bound.getNorthEast().getLatitude());
        }

        parameters.set("locations", HTTP::HTTPUtils::join(values, ","));
    }

    return parameters;
}


FilterStreamingRequestParameters::~FilterStreamingRequestParameters()
{
}


void FilterStreamingRequestParameters::setFollow(const std::string& follow)
{
    setFollows({ follow });
}


void FilterStreamingRequestParameters::setFollows(const std::vector<std::string>& follows)
{
    _follows = follows;
}
    

Poco::Net::NameValueCollection FilterStreamingRequestParameters::toNameValueCollection() const
{
    Poco::Net::NameValueCollection parameters = BaseFilterStreamingRequestParameters::toNameValueCollection();

    if (!_follows.empty()) parameters.set("follow", HTTP::HTTPUtils::join(_follows, ","));

    return parameters;
}


UserStreamingRequestParameters::~UserStreamingRequestParameters()
{
}


void UserStreamingRequestParameters::setReplies(bool replies)
{
    _replies = replies;
}


void UserStreamingRequestParameters::setWith(With with)
{
    _with = with;
}


Poco::Net::NameValueCollection UserStreamingRequestParameters::toNameValueCollection() const
{
    Poco::Net::NameValueCollection parameters = BaseFilterStreamingRequestParameters::toNameValueCollection();

    if (_replies) parameters.set("replies", "all");

    if (_with == With::USERS) parameters.set("with", "users");
    else if (_with == With::FOLLOWINGS) parameters.set("with", "followings");

    return parameters;
}


const std::string SampleStreamingRequest::RESOURCE_URL = "https://stream.twitter.com/1.1/statuses/sample.json";


SampleStreamingRequest::SampleStreamingRequest():
    SampleStreamingRequest(StreamingRequestParameters())
{
}

SampleStreamingRequest::SampleStreamingRequest(const StreamingRequestParameters& parameters):
    HTTP::GetRequest(RESOURCE_URL, Poco::Net::HTTPMessage::HTTP_1_1)
{
    setParameters(parameters);
}


SampleStreamingRequest::~SampleStreamingRequest()
{
}


void SampleStreamingRequest::setParameters(const StreamingRequestParameters& parameters)
{
    _parameters = parameters;

    clearFormFields();

    for (const auto& entry: _parameters.toNameValueCollection())
    {
        setFormField(entry.first, entry.second);
    }
}


StreamingRequestParameters SampleStreamingRequest::getParameters() const
{
    return _parameters;
}



const std::string FilterStreamingRequest::RESOURCE_URL = "https://stream.twitter.com/1.1/statuses/filter.json";



FilterStreamingRequest::FilterStreamingRequest():
    FilterStreamingRequest(FilterStreamingRequestParameters())
{
}


FilterStreamingRequest::FilterStreamingRequest(const FilterStreamingRequestParameters& parameters):
    HTTP::PostRequest(RESOURCE_URL, Poco::Net::HTTPMessage::HTTP_1_1)
{
    setParameters(parameters);
}


FilterStreamingRequest::~FilterStreamingRequest()
{
}


void FilterStreamingRequest::setParameters(const FilterStreamingRequestParameters& parameters)
{
    _parameters = parameters;

    clearFormFields();

    for (const auto& entry: _parameters.toNameValueCollection())
    {
        setFormField(entry.first, entry.second);
    }
}


FilterStreamingRequestParameters FilterStreamingRequest::getParameters() const
{
    return _parameters;
}


} } // namespace ofx::Twitter