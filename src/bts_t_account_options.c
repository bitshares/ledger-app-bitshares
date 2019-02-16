/*******************************************************************************
*  Copyright of the Contributing Authors, including:
*
*   (c) 2019 Christopher J. Sanborn
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
********************************************************************************/

#include "bts_t_account_options.h"
#include "bts_types.h"
#include "os.h"
#include <string.h>

uint32_t deserializeBtsAccountOptionsType(const uint8_t *buffer, uint32_t bufferLength, bts_account_options_type_t * opts) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = deserializeBtsPublicKeyType(buffer, bufferLength, &opts->memoPubkey);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsAccountIdType(buffer, bufferLength, &opts->votingAccount);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = sizeof(uint16_t);
    os_memmove(&opts->numWitnesses, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = sizeof(uint16_t);
    os_memmove(&opts->numCommittee, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsVarint32Type(buffer, bufferLength, &opts->numVotes);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    opts->votes = buffer;

    gobbled = opts->numVotes * sizeof(bts_vote_type_t); // Seek past vote array
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    gobbled = deserializeBtsNullSetType(buffer, bufferLength, &opts->extensions);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    PRINTF("DESERIAL: ACCT_OPTS: Read %d bytes; %d bytes remain\n", read, bufferLength);

    return read;

}

uint32_t deserializeBtsVoteType(const uint8_t *buffer, uint32_t bufferLength, bts_vote_type_t * vote) {

    uint32_t read = 0;
    uint32_t gobbled = 0;

    gobbled = sizeof(uint32_t);
    os_memmove(vote, buffer, gobbled);
    if (gobbled > bufferLength) {
        THROW(EXCEPTION);
    }
    read += gobbled; buffer += gobbled; bufferLength -= gobbled;

    return read;

}

uint32_t prettyPrintBtsVoteType(bts_vote_type_t vote, char * buffer, uint32_t bufferLength) {
    uint32_t written = 0;
    uint32_t vote_type = vote & 0xFF;
    uint32_t vote_inst = vote >> 8;

    snprintf(buffer+written, bufferLength-written, "%u:%u", vote_type, vote_inst);
    written = strlen(buffer);

    return written;
}

uint32_t prettyPrintBtsVotesList(bts_account_options_type_t opts, char * buffer, uint32_t bufferLength) {

    uint32_t written = 0;

    if (opts.numVotes == 0) {
        snprintf(buffer, bufferLength, "(None)");
        written = strlen(buffer);
    } else {
        for (uint32_t i = 0; i < opts.numVotes; i++) {
            bts_vote_type_t tmpVote;
            deserializeBtsVoteType(opts.votes + i * sizeof(bts_vote_type_t), -1, &tmpVote);
            prettyPrintBtsVoteType(tmpVote, buffer+written, bufferLength-written);
            written = strlen(buffer);
            if (i+1 != opts.numVotes) {
                snprintf(buffer+written, bufferLength-written, ",  ");
                written = strlen(buffer);
            } else {
                snprintf(buffer+written, bufferLength-written, " ");
                written = strlen(buffer); // terminal spc sidesteps graphical glitch #uglyhack
            }
        }
    }

    return written;
}
