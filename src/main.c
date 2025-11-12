#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "cmdline.h"
#include "log.h"
#include "json_parser.h"
#include "model_counter.h"

int main(int argc, char *argv[])
{
    struct CommandLine cl = {0};

    log_set_stream(stderr);
    log_set_level(LOG_LEVEL_INFO);

    if (parse_command_line(argc, argv, &cl) != 0)
    {
        LOG_ERROR("Error while processing command-line arguments.");
        return EXIT_FAILURE;
    }

    if (cl.verbose)
    {
        log_set_level(LOG_LEVEL_DEBUG);
        LOG_DEBUG("Verbose mode ON");
    }

    ModelCounter *mc = model_counter_create();
    if (mc == NULL)
    {
        LOG_ERROR("Failed to create ModelCounter");
        return EXIT_FAILURE;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    int rc = 0;

    if (cl.input_file != NULL)
    {
        LOG_INFO("Reading JSON from file: %s", cl.input_file);
        rc = json_parse_file(cl.input_file, mc);
    }
    else
    {
        LOG_INFO("Reading JSON from standard input (stdin)");
        rc = json_parse_fd(STDIN_FILENO, mc);
    }

    if (rc != 0)
    {
        LOG_ERROR("Failed to parse JSON (rc=%d)", rc);
        model_counter_destroy(mc);
        return EXIT_FAILURE;
    }

    LOG_INFO("Printing models and counts:");
    model_counter_print(mc);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) +
                     (t1.tv_nsec - t0.tv_nsec) / 1e9;
    LOG_INFO("Total elapsed time: %.3f seconds", elapsed);

    model_counter_destroy(mc);

    return EXIT_SUCCESS;
}
