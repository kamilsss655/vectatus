/* Copyright 2025 kamilsss655
 * https://github.com/kamilsss655
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include "filter.h"

/// @brief Initialize filter
/// @param filter pointer to filter struct
/// @param attenuation desired attenuation of high frequencies (1-99)
void FILTER_Init(FilterIrrLp_t *filter, uint8_t attenuation)
{
    filter->current_val = 0;
    filter->attenuation = attenuation;
}

/// @brief Performs filter action and returns filtered result
/// @param filter pointer to filter struct
/// @param newInput input value
/// @return filtered value
uint16_t FILTER_Update(FilterIrrLp_t *filter, uint16_t newInput)
{
    filter->current_val = ((filter->current_val * filter->attenuation) + newInput) / (filter->attenuation + 1);
    return filter->current_val;
}