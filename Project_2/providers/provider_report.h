#pragma once
/*
 * providers/provider_report.h
 * ─────────────────────────────────────────────────────────
 *  Звіти для ProviderProfile (уніфіковані дані провайдерів).
 *  Аналогічно report.h, але для не-Steam даних.
 * ─────────────────────────────────────────────────────────
 */

#include "i_provider.h"
#include <vector>

/// Виводить повний звіт одного ProviderProfile у термінал
void reportProviderProfile(const ProviderProfile& profile);

/**
 * Порівнює кілька ProviderProfile між собою.
 * Якщо profiles з різних провайдерів — порівнює
 * час за спільними категоріями.
 */
void reportProviderComparison(const std::vector<ProviderProfile>& profiles);

/**
 * Зводна таблиця: один гравець, всі провайдери.
 * Показує загальний час по кожному провайдеру.
 */
void reportAllProviders(const std::vector<ProviderProfile>& allProfiles);
